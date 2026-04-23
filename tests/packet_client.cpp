/*
    * Simple file copy program in C++.

    Ideia: biblioteca para manipulação, verificar se existe, copiar, validar path: arquivos.


    Gustavo dos Santos Mendes, 16/04/2026.

    argv[0]: -.exe
    argv[1]: <server_ip>
    argv[2]: <port>
    argv[3]: <remote_file_path>
    argv[4]: <local_new_file_path>

*/

#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"
#include "packet.h"

#include "utils.h"


const char* ip = "192.168.1.10";
const int port = 8000;

const char* str = "Gustav0 dos Santos Mendes";

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

    if(ms_connect(sock, ip, port) < 0) {
        fprintf(stderr, "Connection failed.\n");
        return -6;
    }
    
    int32_t rec = send_packet( &sock, (void*)str, strlen(str)+1);

    if(rec < 0){
        fprintf(stderr, "Error in send\n");
        return -11;
    }

    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
