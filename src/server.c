
/*
    * Simple file copy program in C.

    Ideia: biblioteca para manipulação, verificar se existe, copiar, validar path: arquivos.


    Gustavo dos Santos Mendes, 16/04/2026.

    argv[0]: -.exe
    argv[1]: <port>
    argv[2]: <base_folder>


//  Next step:

    argv[0]: server.exe

    argv[1]: <port>

    argv[2]: <base_folder>

*/

// #ifdef __STDC_NO_THREADS__
// #error "Threads não suportadas"
// #endif


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
// #include <pthread.h>

#include "minisocket.h"
#include "packet.h"
#include "packetlib/helpper.h"
#include "filecore.h"
#include "utils.h"

#include "serverHandlers.h"


#define NUMBER_OF_TASKS 5
#define MAX_COMMAND_LENGTH 4000

int validateArguments(int argc, char* argv[]){
    
    if(argc < 3){
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }
    
    if( !isValidPort(argv[1]) ){
        fprintf(stderr, "'%s' is not a valid port number.\n", argv[1]);
        return -2;
    }

    // if(  !isValidDirectory(argv[2]) ){
    //     fprintf(stderr, "Invalid directory path: '%s'\n", argv[2]);
    //     return -4;
    // }

    if(  !existsDirectory(argv[2]) ){
        fprintf(stderr, "Directory not found: '%s'\n", argv[2]);
        return -5;
    }

    return 0;
}

int initServer(ms_socket_t* sock, int port){
    
    if( ms_init() == ms_invalid){
        fprintf(stderr, "Error to initialize socket library.");
        return -1;
    }

    *sock = ms_socket_create();
    
    if(*sock == ms_invalid){
        fprintf(stderr, "Error to create socket.");
        return -2;
    }
    
    if( ms_initServer(*sock, port, NUMBER_OF_TASKS) < 0){
        fprintf(stderr, "Error to create server on socket.");
        return -3;
    }

    return 0;
}

void proceedClientRequest(ms_socket_t sock, Packet* packet, const char* baseFolder){

    uint16_t ret = recvPacket(sock, packet);

    if( ret != 0){
        fprintf(stderr, "Error to receive packet from client [%d:%d]\n", ret, ms_last_error());
        return;
    }

    // The handles can read PACKET and need to fill it to be sent

    switch(packet->header.type){

        case MSG_GET_FILE:      handleGetFileRequest(sock, packet, baseFolder);     break;
        case MSG_PUT_FILE:      handlePutFileRequest(sock, packet, baseFolder);     break;

        case MSG_DELETE_FILE:   handleDeleteFileRequest(sock, packet, baseFolder);  break;
        case MSG_MOVE_FILE:     handleMoveFileRequest(sock, packet, baseFolder);    break;
        case MSG_LIST_FILES:    handleListFileRequest(sock, packet, baseFolder);    break;

        case MSG_CREATE_FOLDER: handleCreateFolderRequest(sock, packet, baseFolder); break;
        case MSG_DELETE_FOLDER: handleDeleteFolderRequest(sock, packet, baseFolder); break;
        case MSG_LIST_FOLDERS:  handleListFolderRequest(sock, packet, baseFolder);  break;
        case MSG_MOVE_FOLDER:   handleMoveFolderRequest(sock, packet, baseFolder);  break;
        
        case MSG_PING_PONG:     handlePingPongRequest(sock, packet); break;
        default:                handlerDefaultRequest(sock, packet); break;

    }

    if( sendPacket(sock, packet) != 0){
        fprintf(stderr, "Error to send END packet to client.\n");
    }
}

int server(ms_socket_t socket, const char* baseFolder){

    ms_socket_t clientSock;

    Packet packet = newPacket(5000);

    int clientPort;
    char clientIP[16];

    while(1){

        printf("\nWaiting for client connection... ");
        clientSock = ms_accept(socket, clientIP, &clientPort);

        if( clientSock == ms_invalid ){
            fprintf(stderr, "Error to accept client connection.\n");
            continue;
        }
        
        printf("Client connected(%s:%d)\n\n", clientIP, clientPort);

        proceedClientRequest(clientSock, &packet, baseFolder);
    }


    return 1;
}

int main(int argc, char* argv[]){

    if( validateArguments(argc, argv) != 0){
        fprintf(stderr, "Invalid input. Use: <-.exe> <port> <dir>\n");
        return -1;
    }

    ms_socket_t sock;
    const int   port   = toNumber(argv[1]);
    char fullBaseFolder[MAX_PATH_LENGTH];

    if( getFullPath(fullBaseFolder, argv[2]) == 0){
        fprintf(stderr, "Error to get full path of base folder.\n");
        return -2;
    }

    if( initServer(&sock, port) != 0){
        fprintf(stderr, "Error to create server on socket[%d].\n", ms_last_error());
        return -3;
    }

    printf("\nServer initialized in: \nPath:\t%s \nPort:\t%d\n\n", fullBaseFolder, port);

    // Verificar se a pasta base existe, caso contrário, criar e avisar

    int result = server(sock, fullBaseFolder);
    
    ms_close(sock);
    ms_cleanup();
    
    return result;
}
