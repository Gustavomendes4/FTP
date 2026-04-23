
#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"

int main(int argc, char* argv[]){

    ms_init();

    ms_socket_t sock = ms_socket_create();

    if(sock == ms_invalid){
        fprintf(stderr, "Could not create a socket\n");
        return -1;
    }

    if( ms_connect(sock, "192.168.1.4", 8000) < 0){
        fprintf(stderr, "Conection failed\n");
        return -2;    
    }

    const char* str = "hi server, tudo bem?? : )";

    ms_send(sock, str, strlen(str));


    ms_close(sock);
    ms_cleanup();
}

