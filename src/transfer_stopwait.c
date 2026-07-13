
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"

//
#include "filecore.h"
#include "utils.h"

#define CHUNCK_SIZE 4096


//  =========   Public functions UDP / stopNwait    =========

#ifdef _WIN32

    #include <windows.h>

#else

    #include <unistd.h>

#endif

#define MAX_RETRANSMISSION  4


//  ========    Private functions   ============

static void serializeU32(uint8_t *buffer, uint32_t value){
    buffer[0] = (uint8_t)(value >> 24);
    buffer[1] = (uint8_t)(value >> 16);
    buffer[2] = (uint8_t)(value >> 8);
    buffer[3] = (uint8_t)value;
}

static uint32_t deserializeU32(const uint8_t *buffer){
    return ((uint32_t)buffer[0] << 24) |
           ((uint32_t)buffer[1] << 16) |
           ((uint32_t)buffer[2] << 8)  |
           (uint32_t)buffer[3];
}

static bool waitAck(ms_socket_t sock, uint32_t expectedPacket){

    uint8_t buffer[4];

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ret = select((int)(sock + 1), &readfds, NULL, NULL, &timeout);

    if (ret <= 0)
        return false;           // timeout ou erro

    ret = recv(sock, (char*)buffer, sizeof(buffer), 0);

    if (ret != sizeof(buffer))
        return false;

    return deserializeU32(buffer) == expectedPacket;
}

static bool sendBuffer_stopNwait(ms_socket_t sock, uint32_t packetNumber, const char* buffer, size_t bufferSize){

    if(!buffer || bufferSize <= 0) return false;

    for(int i = 0; i < MAX_RETRANSMISSION; i++){

        if (ms_send(sock, buffer, bufferSize) != bufferSize)
            continue;


        if (waitAck(sock, packetNumber)) {
            return true;
        }

    }

    return false;

}

static bool recvBuffer_stopNwait(ms_socket_t sock, const char* buffer, size_t bufferSize){

    char buffer[6];



}

static int16_t sendFileInfos_stopNwait(ms_socket_t socket, Packet* packet, FILE* file){

    // setPacketType(packet, MSG_FILE_FOUND);
    // packetWrite(packet, &size, sizeof(size_t));
    // packetWrite(packet, &numC, sizeof(size_t));
    // return sendPacket(socket, packet);

    /*
        0-1: Numero do pacote
        2-3: Tamanho do arquivo
        3-4: Numero de chunks
    */
   
   size_t size = fc_fileSize(file);
   size_t numC = calculateNumChuncks(size);

    clearPacket(packet);

    char buff[6];

    serializeU32(buff, (uint32_t)0);

    serializeU32(buff + 2, (uint32_t)size);

    serializeU32(buff + 4, (uint32_t)numC);

    return sendBuffer_stopNwait(socket, numC, buff, 6);
}

static int16_t recvFileInfos_stopNwait(){

}

int sendFile_stopNwait(ms_socket_t sock, Packet* packet, const char* filePath){

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

int recvFile_stopNwait(ms_socket_t sock, Packet* packet, const char* filePath){

}


//  =========   Public functions UDP / slidingWindow    =========

int recvFile_slidingWindow(ms_socket_t sock, Packet* packet, const char* dstPath){

}

int sendFile_slidingWindow(ms_socket_t sock, Packet* packet, const char* filePath){

}