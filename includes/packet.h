#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>


#include "minisocket.h"

#define PACKETSIZE_VERSION 1
#define MAX_PAYLOAD_SIZE 2048
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
    uint64_t payloadSize; // it is constant
} PacketHeader;

typedef struct{
    uint64_t maxSize;

    uint64_t size;

    uint8_t* buffer;
} PacketPayload;

typedef struct{

PacketHeader header;

PacketPayload payload;

}Packet;

//  ===========    SEND / RECIV    ===============

int32_t recv_buffer(ms_socket_t* socket, void* payload, uint64_t payload_size);

int32_t send_buffer(ms_socket_t* socket, void* payload, uint64_t payload_size);

int32_t recv_packet(ms_socket_t* socket, PacketHeader* header, PacketPayload* payload);

int32_t send_packet(ms_socket_t* socket, PacketHeader* header, PacketPayload* payload);

//  ===========     DEBUG  ===============

void printHeader(PacketHeader head);

//  ===========     HEADER  ===============

PacketHeader newHeader();
PacketHeader newHeaderType(uint32_t);

//  ===========     PACKET  ===============

PacketPayload newPacketPayload(uint8_t* payload, uint64_t max);

// ============= Maior NIVEL: PACKET ================

Packet pck_newPacket(size_t contBytes);

void pck_delPacket(Packet* pck);

//  Validations
int pck_isValidPacket(Packet pack);

// Send / recv
int32_t pck_sendPacket(ms_socket_t* sock, Packet* packet);

int32_t pck_recvPacket(ms_socket_t* sock, Packet* packet);

#endif // PACKET_H_INCLUDED