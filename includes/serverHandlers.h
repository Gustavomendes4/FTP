#ifndef SERVER_HANDLERS_H_INCLUDED
#define SERVER_HANDLERS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "filecore.h"

#include "packetlib/helpper.h"
#include "utils.h"


void handleGetFileRequest(ms_socket_t, Packet*, const char*);

void handlePutFileRequest(ms_socket_t, Packet*, const char*);

void handleDeleteFileRequest(ms_socket_t, Packet*, const char*);

void handleMoveFileRequest(ms_socket_t, Packet*, const char*);

void handleListRequest(ms_socket_t sock, Packet* packet, const char* baseFolder);

void handleCreateFolderRequest(ms_socket_t, Packet*, const char*);

void handleDeleteFolderRequest(ms_socket_t, Packet*, const char*);

void handleListFolderRequest(ms_socket_t, Packet*, const char*);

void handleMoveFolderRequest(ms_socket_t, Packet*, const char*);

void handlePingPongRequest(ms_socket_t);

void handlerDefaultRequest(ms_socket_t);

#endif //SERVER_HANDLERS_H_INCLUDED