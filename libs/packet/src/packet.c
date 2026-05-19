/*
*
*
*
*
*
*
*
*
*
*
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "packetlib/header.h"
#include "packetlib/payload.h"
#include "packet.h"

#include "minisocket.h"

// C:\Users\Gustavo\Desktop\FTP\libs\packet>g++ ./src/packet.c -c -I./includes -I../minisocket


static Packet invalidPacket(){
    Packet pack = {0};

    pack.cursor = 0;

    pack.header.magic = 0;
    pack.header.payloadSize = 0;

    pack.payload.buffer = NULL;
    pack.payload.maxSize = 0;
    return pack;
}

// === Packet Functions ===

Packet newPacket(size_t size){

    Packet pack;
    uint8_t* buffer;

    if(size > MAX_PACKET_PAYLOAD_SIZE){
        return invalidPacket();
    }

    pack.cursor = 0;
    pack.header = newPacketHeader();

    if(size == 0){
        buffer = NULL;
    }
    else{
        buffer = (uint8_t*) malloc(size); 
        
        if(buffer == NULL)
            return invalidPacket();
    }

    pack.payload = newPacketPayload(buffer, size);

    return pack;
}

void delPacket(Packet* pack){
    
    if(pack == NULL) return;

    if(pack->payload.buffer != NULL)
        free(pack->payload.buffer);
    
    *pack = invalidPacket();
}

void clearPacket(Packet* pack){
    
    if(pack == NULL) return;

    pack->cursor = 0;
    
    pack->header = newPacketHeader();
}

int16_t isValidPacket(const Packet *pack){

    int16_t value;
    
    if( pack == NULL)
        return -1;

    value = (int16_t)validatePacketHeader( &(pack->header));

    if(value != 0) return value;

    value = validatePacketPayload( &(pack->payload));

    if(value != 0) return value;
    
    if( pack->header.payloadSize > pack->payload.maxSize )
        return -3;

    return 0;
}

int16_t resizePacket(Packet* pack, size_t newSize){

    if(pack == NULL) return -1;

    if(newSize > MAX_PACKET_PAYLOAD_SIZE) return -2;

    if(newSize == pack->payload.maxSize) return 0; // No need to resize

    if(newSize == 0){
        free(pack->payload.buffer);
        pack->payload.buffer = NULL;
        pack->payload.maxSize = 0;
        pack->header.payloadSize = 0;
        pack->cursor = 0;
        return 0;
    }

    uint8_t* newBuffer = (uint8_t*) realloc(pack->payload.buffer, newSize);

    if(newBuffer == NULL) return -3; // Realloc failed

    pack->payload.buffer = newBuffer;
    pack->payload.maxSize = newSize;

    if(pack->header.payloadSize > newSize){
        pack->header.payloadSize = newSize;
        
        if(pack->cursor > newSize){
            pack->cursor = newSize; // Move cursor back if it exceeds new size
        }
    }

    return 0;
}

// === Send and Recv ===

int16_t sendPacket(ms_socket_t sock, Packet* pack){

    int16_t verify;

    if(pack == NULL){
        return -1;
    }

    // === verify packet ===
    if( isValidPacket(pack) != 0) return -1;


    // == Send header ==
    verify = sendPacketHeader(sock, &(pack->header));

    // == Validate header sent ==
    if(verify < 0) return -2;//verify;


    if( pack->header.payloadSize > 0){

        // == Send payload ==
        verify = sendPacketPayload(sock, &(pack->payload), (pack->header.payloadSize) );
        
        // == Validate payload sent ==
        if(verify < 0) return -3;
    }

    return 0;
}

int16_t recvPacket(ms_socket_t sock, Packet* pack){
    
    if(pack == NULL) return -1;

    int16_t verify;
    PacketHeader tempHead;


    //  Recv Header
    verify = recvPacketHeader(sock, &tempHead);

    // Verify Received Header
    if(verify < 0)
        return -2;


    if( validatePacketHeader(&tempHead) != 0)
        return -3;


    // Verify if exist payload
    const uint64_t size = tempHead.payloadSize;

    if(size == 0){
        pack->header = tempHead;

        pack->cursor = 0;
        return 0; // there are no payload, return success
    }

    
    // Verify if packet size is enough to receive payload
    if( size > pack->payload.maxSize){

        verify = resizePacket(pack, size);

        if( verify < 0)
            return -4; // Failed to resize packet
    }


    // Recv payload
    verify = recvPacketPayload(sock, &(pack->payload), size);

    if( verify < 0){
        return -5; // Failed to receive payload{
    }

    pack->header = tempHead;
    pack->cursor = 0;


    return 0;
}

// === Packet Read / Write ===

uint64_t packetRead( Packet* pack, void* dst, size_t count){

    int16_t valid = isValidPacket(pack);

    if(valid != 0)
        return 0;

    if ( dst == NULL || count == 0 )
        return 0;

    if( pack->cursor >= pack->header.payloadSize )
        return 0; // EOF


    size_t remaining = (size_t)(pack->header.payloadSize - pack->cursor);

    size_t toRead = (count < remaining) ? count : remaining;

    memcpy( dst, pack->payload.buffer + pack->cursor, toRead );

    pack->cursor += toRead;

    return toRead;
}

uint64_t packetWrite(Packet* pack, const void* data, size_t count){


    int16_t valid = isValidPacket(pack);

    if( valid != 0){
        return 0;
    }

    if( data == NULL ){
        return 0;
    }

    if( count > pack->payload.maxSize - pack->cursor ){
        return 0; // Buffer size exceeded
    }


    memcpy( pack->payload.buffer + pack->cursor, data, count );

    pack->cursor += count;

    if( pack->cursor > pack->header.payloadSize )
        pack->header.payloadSize = pack->cursor;

    return count;
}


// === Cursor Control ===

void packetRewind(Packet* pack){
    if(pack != NULL)
        pack->cursor = 0;
}

uint64_t packetTell(const Packet* pack){
    if(pack == NULL)
        return 0;
    
    return pack->cursor;
}

int16_t packetSeek(Packet *pack, long offset, int whence){

    if(pack == NULL)
        return -1;

    int64_t position;

    switch(whence){

        case SEEK_SET:
            position = (int64_t)offset;
            break;

        case SEEK_CUR:
            position = (int64_t)pack->cursor + (int64_t)offset;
            break;

        case SEEK_END:
            position = (int64_t)pack->header.payloadSize + (int64_t)offset;
            break;

        default:
            return -2; // invalid whence
    }

    if(position < 0)
        return -3; // negative position

    if((uint64_t)position > pack->header.payloadSize)
        return -4; // out of bounds

    pack->cursor = (uint64_t)position;

    return 0;
}

uint64_t packetRemaining(const Packet* pack){
    
    if(pack == NULL)
        return 0;

    if(pack->cursor >= pack->header.payloadSize)
        return 0;
    
    return pack->header.payloadSize - pack->cursor;
}

void setPacketType(Packet* pack, uint32_t type){

    if(pack == NULL) return;

    pack->header.type = type;
}

