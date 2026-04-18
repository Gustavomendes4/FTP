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

int isValidPort(const char* port) {
    
    if(!port || port[0] == '\0') return 0;

    if( !isNumber(port) ) return 0;

    int portNum = toNumber(port);

    return (portNum > 0 && portNum <= 65535);
}

int input_validation(int argc, char* argv[]){

    if(argc < 5){
        fprintf(stderr, "Not enough arguments. Usage: cpy <server_ip> <file_path> <new_file_path>\n");
        return -1;
    }

    if( !ms_isValidIp(argv[1]) ){
        fprintf(stderr, "%s is not a valid server IP address.\n", argv[1]);
        return -2;
    }

    if( !isValidPort(argv[2]) ){
        fprintf(stderr, "%s is not a valid port number.\n", argv[2]);
        return -3;
    }

    if( !ms_isValidPath(argv[3]) ){
        fprintf(stderr, "%s is not a valid source file path.\n", argv[2]);
        return -4;
    }

    if( !ms_isValidPath(argv[3]) ){
        fprintf(stderr, "%s is not a valid destination file path.\n", argv[3]);
        return -5;
    }
    
    return 0;
}

int open_safe(FILE** file, const char* path, const char* mode) {
    
    FILE* f = fopen(path, mode);
    if (!f) {
        return -1;
    }

    *file = f;
    return 0;
}

int create_socket(ms_socket_t* sock, char* ip, int port){
    
    if(ms_init() != 0) {
        fprintf(stderr, "Could not initialize Winsock.\n");
        return -10;
    }

    if( (*sock = ms_socket_create()) == ms_invalid){
        fprintf(stderr, "Could not create socket.\n");
        return -5;
    }

    if(ms_connect(*sock, ip, port) < 0) {
        fprintf(stderr, "Connection failed.\n");
        return -6;
    }

}

int main(int argc, char* argv[]){

    /// ========= Validação de argumentos     ========= ///
    if(input_validation(argc, argv) != 0) {
        fprintf(stderr, "Input validation failed.\n");
        return -1;
    }

    /// ========= Validação de arquivos e abertura    ========= ///
    FILE *dst;

    if( open_safe(&dst, argv[3], "wb") != 0 ){
        fprintf(stderr, "Could not open destination file %s\n", argv[3]);
        return -5;
    }

    /// ========= Criação e conexão no socket    ========= ///
    ms_socket_t sock;

    if(create_socket(&sock, argv[1], toNumber(argv[2])) != 0) {
        fprintf(stderr, "Socket creation or connection failed.\n");
        return -10;
    }

    /// ========= |    ========= ///

    /*
    char buffer[BlockSize];
    
    strcpy(request.filePath, argv[2]);
    
    send(sock, (const char*)&request, FileRequestSize, 0);
    
    while(1){
        memset(buffer, 0, sizeof(buffer));
        
        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
        
        if( bytes_received <= 0 )
        break;
        
        fwrite(buffer, sizeof(char), bytes_received, dst);
        
    }
    */

    fclose(dst);
    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
