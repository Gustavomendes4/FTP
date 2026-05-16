#ifndef SERVER_HANDLERS_H_INCLUDED
#define SERVER_HANDLERS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "filecore.h"

#include "helpper.h"
#include "utils.h"


void handleGetFileRequest(ms_socket_t, Packet*);

void handlePutFileRequest(ms_socket_t, Packet*);

void handleDeleteFileRequest(ms_socket_t, Packet*);

void handleMoveFileRequest(ms_socket_t, Packet*);

void handleListFileRequest(ms_socket_t, Packet*);

void handleCreateFolderRequest(ms_socket_t, Packet*);

void handleDeleteFolderRequest(ms_socket_t, Packet*);

void handleListFolderRequest(ms_socket_t, Packet*);

void handleMoveFolderRequest(ms_socket_t, Packet*);

void handlePingPongRequest(ms_socket_t, Packet*);

void handlerDefaultRequest(ms_socket_t, Packet*);

#endif //SERVER_HANDLERS_H_INCLUDED