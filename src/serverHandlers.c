





#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "packetlib/helpper.h"

#include "utils.h"

#include "filecore.h"
#include "transfer.h"

#include "serverHandlers.h"

//
void handleGetFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){

    char completePath[MAX_PATH_LENGTH];
    char receivedPath[MAX_PATH_LENGTH];

    const int baseFolderLen = strlen(baseFolder);
    
    //  ======  Read de payload packet  =======

    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        return;
    }

    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);
    
    // ==========

    if( !isValidPath(completePath) ){
        packetWriteString( packet, MSG_BAD_REQUEST, "Invalid file path." );
        printf(": '%s' is not valid file path\n", completePath);
        return;
    }

    if( !existsFile(completePath) ){
        packetWriteString( packet, MSG_ERROR_FILE_NOT_FOUND, "File not found." );
        printf(": '%s' not found\n", completePath);
        return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        packetWriteString( packet, MSG_ERROR_PERMISSION_DENIED, "Access denied: path is outside the allowed directory." );
        return;
    }

    sendFile(sock, packet, completePath);
}

void handlePutFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handleDeleteFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
    


}

void handleMoveFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handleListFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handleCreateFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handleDeleteFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handleListFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handleMoveFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
}

void handlePingPongRequest(ms_socket_t sock, Packet* packet){
    sendTypedHeader(sock, MSG_PING_PONG);
}

void handlerDefaultRequest(ms_socket_t sock, Packet* packet){
    sendTypedHeader(sock, MSG_BAD_REQUEST); // not verified
}
