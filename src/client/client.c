/*
    * Simple file copy program in C++.

    Ideia: biblioteca para manipulação, verificar se existe, copiar, validar path: arquivos.


    Gustavo dos Santos Mendes, 16/04/2026.

    argv[0]: client.exe
    argv[1]: <server_ip>
    argv[2]: <port>
    argv[3]: <remote_file_path>
    argv[4]: <local_new_file_path>


    - nextstep:
    |   
    |    argv[0]: client.exe
    |    argv[1]: <server_ip>
    |    argv[2]: <port>

    |    argv[3]: <OPERATION>
        | GET_FILE, POST_FILE, DELETE_FILE, LIST_FILE
        | GET_DIR,  POST_DIR, DELETE_DIR, LIST_DIR

    argv[4]: <remote_file_path>
    argv[5]: <local_new_file_path>


*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "packetlib/helpper.h"

#include "transfer.h"
#include "filecore.h"
#include "utils.h"

#include "clientHandlers.h"

OpetarionToken getToken(const char* str){

    OpetarionToken i;

    for(i = 0; i < TK_INVALID_TOKEN; i++){

        if( strcmp(str, Token[i]) == 0)
            return i;
    }

    return TK_INVALID_TOKEN;
}

int validateArguments(int argc, char* argv[]){

    if(argc < 3){
        fprintf(stderr, "Not enough arguments.\n");
        return -1;
    }
    
    if( !isValidIp(argv[1]) ){
        fprintf(stderr, "'%s' is not a valid IP address.\n", argv[1]);
        return -2;
    }

    if( !isValidPort(argv[2]) ){
        fprintf(stderr, "'%s' is not a valid port number.\n", argv[2]);
        return -3;
    }

    // if( getTokn(argv[1]) == TK_INVALID_TOKEN ){
    //     fprintf(stderr, "'%s' is not a valid instruction.\n", argv[2]);
    //     return -3;
    // }

    return 0;
}

int connectToServer(ms_socket_t* sock, const char* serverIp, const int port){

    if( ms_init() == ms_invalid){
        fprintf(stderr, "Error to initialize socket library.");
        return -1;
    }

    *sock = ms_socket_create(MS_TCP);
    
    if(*sock == ms_invalid){
        fprintf(stderr, "Error to create socket.");
        return -2;
    }

    if(ms_connect(*sock, serverIp, port) != 0){
        fprintf(stderr, "Error to connect to server.");
        return -4;
    }

    return 0;
}

int getRequisitionDescriptor(RequestDesc *request, int argc, char* argv[]){

    if( request == NULL){
        fprintf(stderr, "NULL request\n");
        return -1;
    }

    if( argc <= 3){
        fprintf(stderr, "No requisition informed\n");
        return -2;
    }

    /*  ====    Get token from string   ====   */
    request->token = getToken(argv[3]);

    if( request->token == TK_INVALID_TOKEN ){
        fprintf(stderr, "'%s' is not a valid operation.\n", argv[3]);
        return -2;
    }

    // printf(": %d\n", request->token);
    request->arg1 = (argc >= 5) ? argv[4] : NULL;

    request->arg2 = (argc >= 6) ? argv[5] : NULL;

    return 0;
}

int client(ms_socket_t socket, RequestDesc request){

    Packet pack = newPacket(6000);

    switch( request.token ){

        case TK_GET_FILE:   client_handleGetFile(socket, &pack, request);   break;
        case TK_PUT_FILE:   client_handlePutFile(socket, &pack, request);  break;
        case TK_DELETE_FILE:client_handleDeleteFile(socket, &pack, request);break;
        // case TK_LIST_FILE:  client_handleListFile(socket, &pack, request);  break;
        case TK_MOVE_FILE:  client_handleMoveFile(socket, &pack, request);  break;
        // case TK_COPY_FILE:

        case TK_CREATE_DIR:   client_handleCreateDir(socket, &pack, request); break;
        case TK_DELETE_DIR:   client_handleDeleteDir(socket, &pack, request); break;
        case TK_GET_DIR:    client_handleGetDir(socket, &pack, request);    break;
        case TK_PUT_DIR:    client_handlePutDir(socket, &pack, request);    break;
        // case TK_LIST_DIR:   client_handleListDir(socket, &pack, request);   break;
        case TK_MOVE_DIR:   client_handleMoveDir(socket, &pack, request); break;
        case TK_COPY_DIR:   break;

        case TK_PING:       client_handlePingPong(socket, &pack, request);   break;

        default: return -2;
    }

    delPacket(&pack);
    return 0;
    
}

int main(int argc, char* argv[]){

    ms_socket_t sock;
    RequestDesc request;
    
    if(validateArguments(argc, argv) != 0){
        fprintf(stderr, "Invalid input. Use: <-.exe> <server_ip> <port> <operation> <...>\n");
        return -1;
    }

    if(getRequisitionDescriptor(&request, argc, argv) != 0){
        fprintf(stderr, "Invalid requisition. Use: <-.exe> <server_ip> <port> <operation> <...>\n");
        return -1;
    }

    const int port = toNumber(argv[2]);
    printf("Connected to server... ");

    if(connectToServer(&sock, argv[1], port) != 0){
        fprintf(stderr, "Failed to connect to server.\n");
        return -2;
    }

    printf("Done!\n\n");
    
    int ret = client(sock, request);

    ms_close(sock);
    ms_cleanup(sock);

    return ret;
}
