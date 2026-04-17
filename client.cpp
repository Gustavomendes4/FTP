/*
    * Simple file copy program in C++.

    Ideia: biblioteca para manipulação, verificar se existe, copiar, validar path: arquivos.


    Gustavo dos Santos Mendes, 16/04/2026.
*/

#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"

typedef struct FileRequest {
    char filePath[256];


} FileRequest;

#define FileRequestSize sizeof(FileRequest)
#define BlockSize 1024

typedef enum ResposnseCode {
    Success = 0,
    FileNotFound = 1,
    InvalidRequest = 2,
    ServerError = 3
} ResposnseCode;

typedef struct FileResponse {
    uint64_t fileSize;
    char block[BlockSize];

    ResposnseCode code;
} FileResponse;


int main(int argc, char* argv[]){

    FILE *dst;
    
    if(argc < 4){
        printf("Not enough arguments. Usage: cpy <server_ip> <file_path> <new_file_path>\n");
        return -1;
    }

    if( !isValidIp(argv[1]) ){
        printf("%s is not a valid server IP address.\n", argv[1]);
        return -2;
    }

    if( !isValidPath(argv[2]) ){
        printf("%s is not a valid source file path.\n", argv[1]);
        return -3;
    }

    if( !isValidPath(argv[3]) ){
        printf("%s is not a valid destination file path.\n", argv[3]);
        return -3;
    }
    
    if( (dst = fopen(argv[3], "wb")) == NULL){
        printf("Could not open destination file %s\n", argv[3]);
        return -5;
    }


    // Arquivos validados e abertos:

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[BlockSize];

    if(WSAStartup(MAKEWORD(2,2), &wsa)){
        printf("Could not initialize Winsock.\n");
        return -10;
    }

    if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Could not create socket.\n");
        return -5;
    }

    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed.\n");
        return -6;
    }


    FileRequest request;
    
    strcpy(request.filePath, argv[2]);

    send(sock, (const char*)&request, FileRequestSize, 0);

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);

        if( bytes_received <= 0 )
            break;
        
        fwrite(buffer, sizeof(char), bytes_received, dst);

    }

    fclose(dst);
    
    #ifdef _WIN32
        closesocket(sock);
        WSACleanup();
    
    #else
        close(sock);
    #endif

    return 0;
















    printf("Copy completed successfully!\n");
    return 0;
}
