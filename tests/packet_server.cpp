
#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"
#include "packet.h"

#include "utils.h"

int main(int argc, char* argv[]){

    if(ms_init() != 0) {
        fprintf(stderr, "Could not initialize socket library\n");
        return -10;
    }
    
    ms_socket_t sock = ms_socket_create();

    if( sock == ms_invalid){
        fprintf(stderr, "Could not create socket.\n");
        return -5;
    }

    if(ms_initServer(sock, 8000, 5) < 0) {
        fprintf(stderr, "Error to create server.\n");
        return -6;
    }
    
    ms_socket_t s1 = ms_accept(sock, NULL, NULL);
    //=================================================

    char buffer[1000];

    int32_t rec = recv_packet( &s1, (void*)buffer, 1000);
    buffer[999] = '\0';

    if(rec < 0){
        fprintf(stderr, "Error in receive\n");
        return -11;
    }

    printf("Recebido: %s\n", buffer);

    ms_close(s1);
    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
