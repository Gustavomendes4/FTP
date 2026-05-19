#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "minisocket.h"

#include "packetlib/header.h"
#include "packetlib/payload.h"


typedef struct{

    PacketHeader header;

    PacketPayload payload;

    uint64_t cursor;


}Packet;


Packet newPacket(size_t size);

void delPacket(Packet* pack);

void clearPacket(Packet* pack);

int16_t isValidPacket(const Packet *pack);

int16_t resizePacket(Packet* pack, size_t newSize);

int16_t sendPacket(ms_socket_t sock, Packet* pack);

int16_t recvPacket(ms_socket_t sock, Packet* pack);


uint64_t packetRead( Packet* pack, void* dst, size_t count);

uint64_t packetWrite(Packet* pack, const void* data, size_t count);


void packetRewind(Packet* pack);

uint64_t packetTell(const Packet* pack);

int16_t packetSeek(Packet *pack, long offset, int whence);

uint64_t packetRemaining(const Packet* pack);

void setPacketType(Packet* pack, uint32_t type);

#endif //PACKET_H_INCLUDED
