





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


void handleGetFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){

    char completePath[MAX_PATH_LENGTH];
    char receivedPath[MAX_PATH_LENGTH];

    const int baseFolderLen = strlen(baseFolder);
    
    //  ======  Read de payload packet  =======

    if( packet->header.payloadSize + baseFolderLen > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        sendPacket(sock, packet);
        return;
    }

    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);
    
    // ==========

    if( !isValidPath(completePath) ){
        packetWriteString( packet, MSG_BAD_REQUEST, "Invalid file path." );
        sendPacket(sock, packet);
        // printf(": '%s' is not valid file path\n", completePath);
        return;
    }

    if( !existsFile(completePath) ){
        packetWriteString( packet, MSG_ERROR_FILE_NOT_FOUND, "File not found." );
        sendPacket(sock, packet);
        // printf(": '%s' not found\n", completePath);
        return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        packetWriteString( packet, MSG_ERROR_PERMISSION_DENIED, "Access denied: path is outside the allowed directory." );
        sendPacket(sock, packet);
        return;
    }

    sendFile(sock, packet, completePath);
}

void handlePutFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){

    char receivedPath[MAX_PATH_LENGTH];
    char completePath[MAX_PATH_LENGTH];

    const int baseFolderLen = strlen(baseFolder);
    
    //  ======  Read the payload packet  =======

    if( packet->header.payloadSize + baseFolderLen > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        sendPacket(sock, packet);
        printf("(1)\n");return;
    }

    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);
    
    // ==========

    if( !isValidPath(completePath) ){
        packetWriteString( packet, MSG_BAD_REQUEST, "Invalid file path." );
        sendPacket(sock, packet);
        printf("(2)\n");return;
    }

    if( existsFile(completePath) ){
        packetWriteString( packet, MSG_ERROR_FILE_ALREADY_EXISTS, "File already exists" );
        sendPacket(sock, packet);
        printf("(3)\n");return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        packetWriteString( packet, MSG_ERROR_PERMISSION_DENIED, "Access denied: path is outside the allowed directory." );
        sendPacket(sock, packet);
        return;
    }

    recvFile(sock, packet, completePath);
}

void handleDeleteFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
    char receivedPath[MAX_PATH_LENGTH];
    char completePath[MAX_PATH_LENGTH];

    const int baseFolderLen = strlen(baseFolder);

    //  ======  Read de payload packet  =======

    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        sendPacket(sock, packet);
        return;
    }

    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);
    
    // ==========

    if( !isValidPath(completePath) ){
        packetWriteString( packet, MSG_BAD_REQUEST, "Invalid file path." );
        sendPacket(sock, packet);
        return;
    }

    if( !existsFile(completePath) ){
        packetWriteString( packet, MSG_ERROR_FILE_NOT_FOUND, "File not found." );
        sendPacket(sock, packet);
        return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        packetWriteString( packet, MSG_ERROR_PERMISSION_DENIED, "Access denied: path is outside the allowed directory." );
        sendPacket(sock, packet);
        return;
    }

    // Try to delete
    if(fc_delete(completePath) == 0)
        sendTypedHeader(sock, MSG_OK);
    else
        sendTypedHeader(sock, MSG_ERROR);

}

void handleMoveFileRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
    char msg[MAX_PATH_LENGTH];

    char filepath[MAX_PATH_LENGTH];
    char dstpath[MAX_PATH_LENGTH];
    char complete_filepath[MAX_PATH_LENGTH];
    char complete_dstpath[MAX_PATH_LENGTH];

    char newFilePath[MAX_PATH_LENGTH];


    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH * 2){
        clearPacket(packet);
        sendPacketString(sock, MSG_BAD_REQUEST, "File path is too long!");
        return;
    }

    /// ======  Read PATH   ========
    packetReadStr(packet, filepath, MAX_PATH_LENGTH);
    
    packetReadStr(packet, dstpath, MAX_PATH_LENGTH);
    
    clearPacket(packet);

    /// ======  Mount complete PATHs   ========
    buildPath(complete_filepath, MAX_PATH_LENGTH, baseFolder, filepath);
    getFullPath(complete_filepath, complete_filepath);

    buildPath(complete_dstpath, MAX_PATH_LENGTH, baseFolder, dstpath);
    getFullPath(complete_dstpath, complete_dstpath);

    buildPath(newFilePath, MAX_PATH_LENGTH, complete_dstpath, fc_getName(complete_filepath));
    
    /// ======  Validate    ==========
    
    if( !isValidPath(complete_filepath) ){
        sprintf(msg, "<%s> is not a valid path", filepath);
        sendPacketString(sock, MSG_BAD_REQUEST, msg);
        return;
    }
    
    if( !isValidPath(complete_dstpath) ){
        sprintf(msg, "<%s> is not a valid path", dstpath);
        sendPacketString(sock, MSG_BAD_REQUEST, msg);
        return;
    }
    
    if( !existsFile(complete_filepath) ){
        sprintf(msg, "<%s> not found", filepath);
        sendPacketString(sock, MSG_ERROR_FILE_NOT_FOUND, msg);
        return;
    }

    if( !existsDirectory(complete_dstpath) ){
        sprintf(msg, "<%s> not found", dstpath);
        sendPacketString(sock, MSG_ERROR_FOLDER_NOT_EXISTS, msg);
        return;
    }

    if( existsFile(newFilePath)){
        sprintf(msg, "<%s> already exist", dstpath);
        sendPacketString(sock, MSG_ERROR_FILE_ALREADY_EXISTS, msg);
        return;
    }
        
    if( !pathBelongsToRoot(complete_filepath, baseFolder) ){
        sprintf(msg, "<%s> Path is outside the allowed directory.", filepath);
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, msg);
        return;
    }

    if( !pathBelongsToRoot(complete_dstpath, baseFolder) ){
        sprintf(msg, "<%s> Path is outside the allowed directory.", dstpath);
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, msg);
        return;
    }


    // ========         Rename it       ========
    if( fc_move(complete_filepath, complete_dstpath)){
        sendTypedHeader(sock, MSG_OK);
    }
    else{
        sendTypedHeader(sock, MSG_ERROR);
    }
}


/**/void handleListRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
    char receivedPath[MAX_PATH_LENGTH];
    char completePath[MAX_PATH_LENGTH];

    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        sendPacket(sock, packet);
        return;
    }

    /// ======  Read PATH   ========
    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);


    /// ======  Validate    ==========
    if( !existsDirectory(completePath) ){
        sendTypedHeader(sock, MSG_ERROR_FOLDER_NOT_EXISTS);
        return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, "Path is outside the allowed directory.");
        return;
    }

    //...
}

void handleCreateFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
    char receivedPath[MAX_PATH_LENGTH];
    char completePath[MAX_PATH_LENGTH];

    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        sendPacket(sock, packet);
        return;
    }

    /// ======  Read PATH   ========
    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);


    /// ======  Validate    ==========
    if( existsDirectory(completePath) ){
        sendTypedHeader(sock, MSG_ERROR_FOLDER_ALREADY_EXISTS);
        return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, "Path is outside the allowed directory.");
        return;
    }

    if( fc_createFolder(completePath) ){
        sendTypedHeader(sock, MSG_OK);
    }
    else{
        sendTypedHeader(sock, MSG_ERROR);
    }
}

void handleDeleteFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){
    
    char receivedPath[MAX_PATH_LENGTH];
    char completePath[MAX_PATH_LENGTH];

    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH ){
        clearPacket(packet);
        packetWriteString( packet, MSG_BAD_REQUEST, "File path is too long!" );
        sendPacket(sock, packet);
        return;
    }

    /// ======  Read PATH   ========
    packetReadStr(packet, receivedPath, MAX_PATH_LENGTH);
    clearPacket(packet);

    buildPath(completePath, MAX_PATH_LENGTH, baseFolder, receivedPath);
    getFullPath(completePath, completePath);


    /// ======  Validate    ==========
    if( !existsDirectory(completePath) ){
        sendTypedHeader(sock, MSG_ERROR_FOLDER_NOT_EXISTS);
        return;
    }

    if( !pathBelongsToRoot(completePath, baseFolder) ){
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, "Path is outside the allowed directory.");
        return;
    }

    if( !fc_isFolderEmpty(completePath)){
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, "The folder is not empty");
        return;    
    }

    if( fc_deleteFolder(completePath) ){
        sendTypedHeader(sock, MSG_OK);
    }
    else{
        sendTypedHeader(sock, MSG_ERROR);
    }
}

void handleMoveFolderRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){

    char msg[MAX_PATH_LENGTH];

    char srcpath[MAX_PATH_LENGTH];
    char dstpath[MAX_PATH_LENGTH];
    char complete_srcpath[MAX_PATH_LENGTH];
    char complete_dstpath[MAX_PATH_LENGTH];

    if( packet->header.payloadSize + strlen(baseFolder) > MAX_PATH_LENGTH * 2){
        clearPacket(packet);
        sendPacketString(sock, MSG_BAD_REQUEST, "File path is too long!");
        return;
    }

    /// ======  Read PATH   ========
    packetReadStr(packet, srcpath, MAX_PATH_LENGTH);
    
    packetReadStr(packet, dstpath, MAX_PATH_LENGTH);
    
    clearPacket(packet);

    /// ======  Mount complete PATHs   ========
    buildPath(complete_srcpath, MAX_PATH_LENGTH, baseFolder, srcpath);
    getFullPath(complete_srcpath, complete_srcpath);

    buildPath(complete_dstpath, MAX_PATH_LENGTH, baseFolder, dstpath);
    getFullPath(complete_dstpath, complete_dstpath);
    
    /// ======  Validate    ==========
    
    if( !isValidPath(complete_srcpath) ){
        sprintf(msg, "<%s> is not a valid path", srcpath);
        sendPacketString(sock, MSG_BAD_REQUEST, msg);
        return;
    }
    
    if( !isValidPath(complete_dstpath) ){
        sprintf(msg, "<%s> is not a valid path", dstpath);
        sendPacketString(sock, MSG_BAD_REQUEST, msg);
        return;
    }
    
    if( !existsDirectory(complete_srcpath) ){
        sprintf(msg, "<%s> not found", srcpath);
        sendPacketString(sock, MSG_ERROR_FOLDER_NOT_EXISTS, msg);
        return;
    }

    if( existsDirectory(complete_dstpath) ){
        sprintf(msg, "Folder <%s> already exists", dstpath);
        sendPacketString(sock, MSG_ERROR_FOLDER_ALREADY_EXISTS, msg);
        return;
    }
        
    if( !pathBelongsToRoot(complete_srcpath, baseFolder) ){
        sprintf(msg, "<%s> Path is outside the allowed directory.", srcpath);
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, msg);
        return;
    }

    if( !pathBelongsToRoot(complete_dstpath, baseFolder) ){
        sprintf(msg, "<%s> Path is outside the allowed directory.", dstpath);
        sendPacketString(sock, MSG_ERROR_PERMISSION_DENIED, msg);
        return;
    }

    // ========         Rename it       ========
    if( fc_moveFolder(complete_srcpath, complete_dstpath)){
        sendTypedHeader(sock, MSG_OK);
    }
    else{
        sendTypedHeader(sock, MSG_ERROR);
    }
}

void handlePingPongRequest(ms_socket_t sock){
    sendTypedHeader(sock, MSG_PING_PONG);
}

void handlerDefaultRequest(ms_socket_t sock){
    sendTypedHeader(sock, MSG_BAD_REQUEST);
}
