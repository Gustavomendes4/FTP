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
#include <stdint.h>

#include "packetlib/payload.h"

// C:\Users\Gustavo\Desktop\FTP\libs\packet>g++ ./src/payload.c -c -I./includes -I../minisocket


PacketPayload newPacketPayload(uint8_t* buffer, uint64_t max){
    PacketPayload ret;

    ret.buffer = buffer;
 
    ret.maxSize = (max <= MAX_PACKET_PAYLOAD_SIZE) ? max : MAX_PACKET_PAYLOAD_SIZE;

    return ret;
}

int16_t validatePacketPayload( const PacketPayload* payload){

    if(payload == NULL)
        return -7;

    if(payload->maxSize > MAX_PACKET_PAYLOAD_SIZE)
        return -8;

    if(payload->maxSize > 0 && payload->buffer == NULL)
        return -10;

    return 0;
}

int16_t sendPacketPayload(ms_socket_t sock, const PacketPayload* pack, uint64_t size){

    if(pack == NULL)
        return -1;

    if(pack->buffer == NULL)
        return -2;

    if( validatePacketPayload(pack) != 0)
        return -3;

    if(size > pack->maxSize)
        return -4;

    int bytes_sent = ms_send_all(sock, pack->buffer, size);

    if(bytes_sent == 0)
        return -1;

    if(bytes_sent < 0)
        return -2;

    return 0;
}

int16_t recvPacketPayload(ms_socket_t sock, PacketPayload* pack, uint64_t size){

    if(pack == NULL)
        return -1;

    if(pack->buffer == NULL)
        return -2;

    if( validatePacketPayload(pack) != 0)
        return -3;

    if(pack->maxSize < size)
        return -4; //not enough buffer size


    int bytes_recvd = ms_recv_all(sock, pack->buffer, size);

    if(bytes_recvd == 0)
        return -1;

    if(bytes_recvd < 0)
        return -2;

    return 0;
}

