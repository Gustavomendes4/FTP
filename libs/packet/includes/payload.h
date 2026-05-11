

#if !defined PACKET_H_INCLUDED && DISABLED_ERROR_COMPILATION
    #error "never include packet/header.h, user only packet.h"
#endif //PACKET_H_INCLUDED

#ifndef PACKET_PAYLOAD_H_INCLUDED
#define PACKET_PAYLOAD_H_INCLUDED



#define MAX_PACKET_PAYLOAD_SIZE 4096

#include <stdint.h>
#include <stdlib.h>

#include "minisocket.h"

typedef struct{
    uint64_t maxSize;
    uint8_t* buffer;

    // uint64_t size;

} PacketPayload;


PacketPayload newPacketPayload(uint8_t* buffer, uint64_t max);

int16_t validatePacketPayload( const PacketPayload* payload);

int16_t sendPacketPayload(ms_socket_t sock, const PacketPayload* pack, uint64_t size);

int16_t recvPacketPayload(ms_socket_t sock, PacketPayload* pack, uint64_t size);

#endif //#ifndef PACKET_PAYLOAD_H_INCLUDED