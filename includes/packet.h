#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <stdint.h>

#include "minisocket.h"

#define MAX_PAYLOAD_SIZE 2048

#define PACKET_MAGIC_NUMBER 0xBABE

// FLAGS
#define FLAG_NONE   0x00
#define FLAG_EOF    0x01
#define FLAG_ERR    0x02

typedef enum{
    MSG_EMPTY = 0,

    //Request
    MSG_GET_FILE,
    MSG_PUT_FILE,
    MSG_DELETE_FILE,
    MSG_UPDATE_FILE,
    MSG_LIST_FILES,

    //Response
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

typedef struct{
    uint64_t maxSize;
    uint8_t* buffer;
} Packet;

int32_t recv_packet(ms_socket_t* socket, void* payload, uint64_t payload_size);

int32_t send_packet(ms_socket_t* socket, void* payload, uint64_t payload_size);



#endif // PACKET_H_INCLUDED