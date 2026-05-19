#ifndef PACKET_HELPER_H_INCLUDED
#define PACKET_HELPER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"

uint16_t sendTypedHeader(ms_socket_t sock, uint32_t  type);

uint16_t sendPacketString(ms_socket_t sock, uint32_t type, const char* str);

uint64_t packetReadStr(Packet* packet, uint8_t* buffer, size_t maxSize);

uint64_t packetSendUint64(Packet* packet, uint64_t value);

uint64_t packetReadUint64(Packet* packet, uint64_t* value);

uint64_t packetWriteString(Packet* packet, uint32_t type, const char* str);

uint32_t getPacketType(Packet* packet);

#endif