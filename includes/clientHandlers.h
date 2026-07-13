#ifndef CLIENT_HANDLERS_H_INCLUDED
#define CLIENT_HANDLERS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "filecore.h"

#include "packetlib/helpper.h"
#include "utils.h"

extern const char *Token[];

typedef enum{
    
    TK_GET_FILE     = 0,
    TK_PUT_FILE,
    TK_DELETE_FILE,
    TK_LIST_FILE,
    TK_MOVE_FILE,
    TK_COPY_FILE,

    TK_CREATE_DIR,
    TK_DELETE_DIR,
    TK_GET_DIR,
    TK_PUT_DIR,
    TK_LIST_DIR,
    TK_MOVE_DIR,
    TK_COPY_DIR,

    TK_PING,
    
    TK_INVALID_TOKEN

}OpetarionToken;

typedef struct RequestDesc{

    OpetarionToken token;

    char* arg1;

    char* arg2;

}RequestDesc;

void client_handleGetFile(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handlePutFile(ms_socket_t socket, Packet* pack, RequestDesc request); 
void client_handleDeleteFile(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handleListFile(ms_socket_t socket, Packet* pack, RequestDesc request);
// case TK_MOVE_FILE:
// case TK_COPY_FILE:

void client_handleCreateDir(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handleDeleteDir(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handleGetDir(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handlePutDir(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handleList(ms_socket_t socket, Packet* pack, RequestDesc request);
void client_handleMoveDir(ms_socket_t socket, Packet* pack, RequestDesc request);
// case TK_COPY_DIR:   break;

void client_handlePingPong(ms_socket_t socket, Packet* pack, RequestDesc request);

void client_handleMoveFile(ms_socket_t socket, Packet* pack, RequestDesc request);

#endif