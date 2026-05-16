





#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "filecore.h"

#include "helpper.h"
#include "utils.h"

#include "serverHandlers.h"

void handleGetFileRequest(ms_socket_t sock, Packet* packet){

}

void handlePutFileRequest(ms_socket_t sock, Packet* packet){
    
}

void handleDeleteFileRequest(ms_socket_t sock, Packet* packet){
    
}

void handleMoveFileRequest(ms_socket_t sock, Packet* packet){
    
}

void handleListFileRequest(ms_socket_t sock, Packet* packet){
    
}

void handleCreateFolderRequest(ms_socket_t sock, Packet* packet){
    
}

void handleDeleteFolderRequest(ms_socket_t sock, Packet* packet){
    
}

void handleListFolderRequest(ms_socket_t sock, Packet* packet){
    
}

void handleMoveFolderRequest(ms_socket_t sock, Packet* packet){
    
}

void handlePingPongRequest(ms_socket_t sock, Packet* packet){
    sendTypedHeader(sock, MSG_PING_PONG);
}

void handlerDefaultRequest(ms_socket_t sock, Packet* packet){
    sendTypedHeader(sock, MSG_BAD_REQUEST); // not verified
}
