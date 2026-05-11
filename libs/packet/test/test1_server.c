

#include <stdio.h>
#include <stdlib.h>

#include "minisocket.h"
#include "packet.h"

int main(int argc, char* argv[]){

    ms_init();

    ms_socket_t sock = ms_socket_create();

    if(sock == ms_invalid){
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    if(ms_initServer(sock, 8000, 5) == ms_invalid){
        fprintf(stderr, "Failed to initialize server\n");
        return -2;
    }

    printf("Server initialized on port 8000, waiting for connections...\n");

    ms_socket_t clientSock = ms_accept(sock, NULL, NULL);

    if( clientSock == ms_invalid){
        fprintf(stderr, "Failed to accept connection\n");
        return -3;
    }

    
    printf("Client connected, waiting for packet...\n");

    Packet packet = newPacket(500);

    int v = (int)recvPacket( clientSock, &packet);
    printf("Packet received, processing...[%d]\n", v);

    char buffer[5600];
    packetRead(&packet, buffer, sizeof(buffer));

    printf("Received message: %s\n", buffer);

}
