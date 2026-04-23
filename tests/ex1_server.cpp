

#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"

int main(int argc, char* argv[]){

    ms_init();

    ms_socket_t sock = ms_socket_create();

    ms_initServer(sock, 8000, 5);

    printf("waiting connection...\n");

    int in;
    char str[30];

    ms_socket_t client = ms_accept(sock, str, &in);

    if(client == ms_invalid){
        fprintf(stderr, "accept error\n");
        return -1;
    }

    printf("Connect successfully: ip(%s) | port(%d)\n", str, in);

    char buff[1001];
    int byte = ms_recv(client, buff, sizeof(buff)-1);
    

    if(byte <= 0){
        fprintf(stderr, "recived error\n");
        return -2;
    }

    buff[byte] = '\0';

    printf("Recived: %s\n", buff);

    ms_close(sock);
    ms_close(client);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
