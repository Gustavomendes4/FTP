

#include <stdio.h>
#include <stdlib.h>

#include "minisocket.h"
#include "packet.h"

int main(int argc, char* argv[]){

    ms_init();

    ms_socket_t sock = ms_socket_create();


    if( ms_connect(sock, "127.0.0.1", 8000) ){
        fprintf(stderr, "Failed to connect to server\n");
        return -1;
    }


    Packet packet = newPacket(500);

    printf("valid = %d\n", (int)isValidPacket(&packet));
    // manipulação do packet

    const char* str = "Hello, Server!";

    uint64_t bytes = packetWrite(&packet, str, strlen(str) + 1);

    printf("write: %llu bytes\n", bytes);

    int v = (int)sendPacket(sock, &packet);

    printf("sendPacket returned: %d\n", v);

}
