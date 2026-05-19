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


    return 0;
}

int connectToServer(ms_socket_t* sock, const char* serverIp, int port){

    if( ms_init() == ms_invalid){
        fprintf(stderr, "Error to initialize socket library.");
        return -1;
    }

    *sock = ms_socket_create();
    
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

int client(ms_socket_t socket){

    Packet request = newPacket(250);

    if( sendPacketString(socket, MSG_GET_FILE, "/./ola.txt") != 0){
        fprintf(stderr, "Error to send packet to server.\n");
        return -1;
    }

    // size_t fileSize, numChunks;

    // int16_t result = recvFileInfos(socket, &request, &fileSize, &numChunks);

    int result = recvFile(socket, &request, "../../../recebido.txt");


    if(  result != 0 ){
        fprintf(stderr, "error in recv file [%d]", (int)result);
        return -3;
    }
    
    return 0;
}

int main(int argc, char* argv[]){

    if(validateArguments(argc, argv) != 0){
        fprintf(stderr, "Invalid input. Use: <-.exe> <server_ip> <port> <> <>\n");
        return -1;
    }

    ms_socket_t sock;

    int port = toNumber(argv[2]);

    printf("Connected to server... ");

    if(connectToServer(&sock, argv[1], port) != 0){
        fprintf(stderr, "Failed to connect to server.\n");
        return -2;
    }

    printf("Done!\n\n");

    return client(sock);
}
