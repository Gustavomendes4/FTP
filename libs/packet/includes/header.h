#if !defined PACKET_H_INCLUDED && DISABLED_ERROR_COMPILATION
    #error "never include packet/header.h, user only packet.h"
#endif //PACKET_H_INCLUDED

#ifndef PACKET_HEADER_H_INCLUDED
#define PACKET_HEADER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

#include "minisocket.h"

#define PACKET_CURR_VERSION 1
// #define MAX_PAYLOAD_SIZE 2048
#define PACKET_MAGIC_NUMBER 0xBABE

// FLAGS
#define FLAG_NONE   0x00
#define FLAG_EOF    0x01
#define FLAG_ERR    0x02

typedef enum{
    MSG_EMPTY = 0,
    MSG_DEFAULT,

    //Request
    MSG_GET_FILE,
    MSG_PUT_FILE,
    MSG_DELETE_FILE,
    MSG_UPDATE_FILE,
    MSG_LIST_FILES,

    //Response
    MSG_FILE_FOUND, // ok, vou mandar
    MSG_FILE_CONTENT,
    MSG_FILE_LIST,
    MSG_ACK,

    MSG_ERROR,
    MSG_ERROR_FILE_NOT_FOUND,
    MSG_ERROR_PERMISSION_DENIED,
    MSG_ERROR_INVALID_REQUEST

}MessageType;

typedef struct {
    uint16_t magic;
    uint16_t version;

    uint32_t type;
    uint32_t flags;
    uint64_t payloadSize;
} PacketHeader;

#define PACKET_HEADER_SIZE (20)


int validatePacketHeader(const PacketHeader* header);

void printHeader(PacketHeader head);

PacketHeader newPacketHeader();

PacketHeader newPacketHeaderType(uint32_t);

int16_t sendPacketHeader(ms_socket_t sock, const PacketHeader* header);

int16_t recvPacketHeader(ms_socket_t sock, PacketHeader* header);


#endif //PACKET_HEADER_H_INCLUDED
