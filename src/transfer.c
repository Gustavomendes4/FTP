
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "packetlib/helpper.h"

#include "filecore.h"
#include "utils.h"
#include "transfer.h"


static size_t calculateNumChuncks(size_t fileSize){
    size_t numChuncks = fileSize / CHUNCK_SIZE;
    
    if( fileSize % CHUNCK_SIZE != 0)
        numChuncks++;

    return numChuncks;
}

static size_t readChunk(uint8_t* chunk, FILE* file){

    if(chunk == NULL) return 0;

    return fread(chunk, sizeof(chunk[0]), CHUNCK_SIZE, file);

}

static int recvFail(FILE* file, const char* path, int code){

    if(file != NULL)
        fc_close(file);

    fc_delete(path);

    return code;
}

static size_t writeChunk(uint8_t* chunk, size_t count, FILE* file){

    if(chunk == NULL) return 0;

    return fwrite(chunk, sizeof(chunk[0]), count, file); 
}

//  =========   Public functions    =========

int16_t sendFileInfos(ms_socket_t sock, Packet* packet, FILE* file){

    size_t size = fc_fileSize(file);
    size_t numC = calculateNumChuncks(size);

    clearPacket(packet);

    setPacketType(packet, MSG_FILE_FOUND);

    packetWrite(packet, &size, sizeof(size_t));
    packetWrite(packet, &numC, sizeof(size_t));

    return sendPacket(sock, packet);
}

int16_t recvFileInfos(ms_socket_t sock, Packet* packet, size_t* filesize, size_t* numChunks){
    
    if(!packet || ! filesize || !numChunks)
        return -10;

    int16_t result = recvPacket(sock, packet);

    if(result < 0)
        return result;

    if( getPacketType(packet) != MSG_FILE_FOUND){
        // printf(": msg num (%d)", (int)getPacketType(packet));
        return -20;
    }


    if( packetRead(packet, filesize, sizeof(size_t)) == 0 )
        return -4;
    
    if( packetRead(packet, numChunks, sizeof(size_t)) == 0 )
        return -5;

    return 0;
}

int sendFile(ms_socket_t sock, Packet* packet, const char* filePath){

    FILE* file;
    
    // ====  Open file and get infos  ====
    fc_open(&file, filePath, "rb");

    if( file == NULL) return -1;

    const size_t size       = fc_fileSize(file);
    const size_t numChunks  = calculateNumChuncks(size);

    //====  Send file infos  ====
    if( sendFileInfos(sock, packet, file) < 0){
        fprintf(stderr, "Erro to send file infos");
        fc_close(file);
        return -2;
    }

    // ====  send file ====
    uint8_t chunk[CHUNCK_SIZE];
    size_t total = 0;

    for( size_t i = 0; i < numChunks; i++){
        
        size_t readed = readChunk(chunk, file);

        clearPacket(packet);
        setPacketType(packet, MSG_FILE_CHUNK);
        
        if(packetWrite(packet, chunk, readed) != readed){
            // fprintf(stderr, "Failed to write chunk to packet [%d]\n", (int)  total + readed);
            fprintf(stderr, "Failed to write chunk to packet [%llu]\n", (unsigned long long)(total + readed));

            clearPacket(packet);

            fc_close(file);
            return -5;
        }


        int16_t result = sendPacket(sock, packet);

        if( result < 0){
            fprintf(stderr, "Error sending chunk: %d (%llu bytes sent)\n", (int) result, (unsigned long long)total);
            
            clearPacket(packet);
            packetWriteString( packet, MSG_ERROR, "Error on chunk sent" );

            fc_close(file);
            return -3;
        }
        total += readed;
    }

    fc_close(file);
    clearPacket(packet);

    if( total != size){
        fprintf(stderr, "Failure in transmission[ sent: %llu | size: %llu] \n", (unsigned long long)total, (unsigned long long)size);

        packetWriteString( packet, MSG_ERROR, "File size mismatch during transmission" );
        return -4;
    }
    
    sendTypedHeader(sock, MSG_EOF);

    return 0;
}

int recvFile(ms_socket_t sock, Packet* packet, const char* dstPath){

    FILE* file;

    fc_open(&file, dstPath, "wb");
    
    if( file == NULL) return -1;
    
    size_t fsize, numChunks;

    //====  Recv file infos  ====
    if( recvFileInfos(sock, packet, &fsize, &numChunks) < 0){
        // fprintf(stderr, "Error receiving file infos\n");
        fc_close(file); 
        return -2;
    }


    uint8_t chunk[CHUNCK_SIZE];
    size_t total = 0;

    for(size_t i = 0; i < numChunks; i++){

        // ==== Receive pakcet ====
        int16_t result = recvPacket(sock, packet);

        if( result < 0){
            fprintf(stderr, "Error receiving chunk[%d]. \n", (int) result);
            return recvFail(file, dstPath, -3);
        }

        if(getPacketType(packet) != MSG_FILE_CHUNK){
            fprintf(stderr, "Expected FILE_CHUNK packet.\n");
            return recvFail(file, dstPath, -8);
        }

        // ==== Read pakcet ====
        uint64_t remaining = packetRemaining(packet);

        if(remaining > CHUNCK_SIZE){
            fprintf(stderr, "Chunk too large.\n");
            return recvFail(file, dstPath, -7);
        }

        uint64_t received = packetRead(packet, chunk, remaining);

        if( received == 0){
            fprintf(stderr, "Failed to read chunk in packet\n");
            return recvFail(file, dstPath, -5);
        }

        size_t writed = writeChunk(chunk, (size_t)received, file);

        if(writed != (size_t)received ){
            fprintf(stderr, "Error on write on chunk\n");
            return recvFail(file, dstPath, -6);
        }

        total += writed;
    }

    fc_close(file);

    if( total != fsize){
        fprintf(stderr, "Failure in transmission[ received: %llu | size: %llu] \n", (unsigned long long)total, (unsigned long long)fsize);
        fc_delete(dstPath);
        return -4;
    }
    
    // try to receive EOF packet
    int16_t result = recvPacket(sock, packet);

    if(result < 0){
        fprintf(stderr, "Error: expected a EOF msg.");
        return -10;
    }

    if( getPacketType(packet) != MSG_EOF ){
        fprintf(stderr, "End msg should be EOF (recved %d)", (int)getPacketType(packet));
        return -11;
    }

    return 0;
}
