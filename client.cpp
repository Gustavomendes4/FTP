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

#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"
#include "packet.h"

#include "utils.h"

static uint64_t htonll(uint64_t v){
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return ((uint64_t)htonl((uint32_t)(v >> 32))) |
           ((uint64_t)htonl((uint32_t)(v & 0xFFFFFFFF)) << 32);
#else
    return v;
#endif
}

static uint64_t ntohll(uint64_t v){
    return htonll(v);
}


int input_validation(int argc, char* argv[]){

    if(argc < 5){
        fprintf(stderr, "Not enough arguments.\nUsage: cpy <server_ip> <server_port> <file_path> <new_file_path>\n");
        return -1;
    }

    if( !isValidIp(argv[1]) ){
    // if( !ms_isValidIp(argv[1]) ){
        fprintf(stderr, "%s is not a valid server IP address.\n", argv[1]);
        return -2;
    }

    if( !isValidPort(argv[2]) ){
        fprintf(stderr, "%s is not a valid port number.\n", argv[2]);
        return -3;
    }

    if( !ms_isValidPath(argv[3]) ){
        fprintf(stderr, "%s is not a valid source file path.\n", argv[3]);
        return -4;
    }

    if( !ms_isValidPath(argv[4]) ){
        fprintf(stderr, "%s is not a valid destination file path.\n", argv[4]);
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

int create_socket(ms_socket_t* sock, const char* ip, int port){

    if(ms_init() != 0) {
        fprintf(stderr, "Could not initialize socket library\n");
        return -10;
    }

    if( (*sock = ms_socket_create()) == ms_invalid){
        fprintf(stderr, "Could not create socket.\n");
        return -5;
    }

    if( (ms_connect(*sock, ip, port)) < 0) {
        fprintf(stderr, "Connection failed.\n");
        return -6;
    }

    return 0;
}

int main(int argc, char* argv[]){
    
    /// ========= Validação de argumentos     ========= ///
    if(input_validation(argc, argv) != 0) {
        fprintf(stderr, "Input validation failed.\n");
        return -1;
    }

    /// ========= Validação de arquivos e abertura    ========= ///
    FILE *dst;

    if( open_safe(&dst, argv[4], "wb") != 0 ){
        fprintf(stderr, "Could not open destination file %s\n", argv[4]);
        return -5;
    }

    /// ========= Criação e conexão no socket    ========= ///
    ms_socket_t sock;

    if(create_socket(&sock, argv[1], toNumber(argv[2])) != 0) {
        fprintf(stderr, "Socket creation or connection failed.\n");
        //close socket and file .
        return -10;
    }

    /// ========= | Envia soliciatação de arquivo |   ========= ///
    int32_t ret;

    PacketHeader headSend = newHeaderType(MSG_GET_FILE);
    headSend.payloadSize = strlen(argv[3]) + 1;

    PacketPayload packetSend = newPacketPayload((uint8_t*)argv[3], strlen(argv[3])+1);
    
    ret = send_packet(&sock, &headSend, &packetSend);
    
    if(ret < 0){
        fprintf(stderr, "Error to send path\n");
        //close socket and file .
        return -11;
    }
    
    /// ========= |      Recebe  resposnse   |   ========= ///
    
    #define BUFFER_SIZE 1450
    uint8_t buffer[BUFFER_SIZE];
    PacketPayload packet = newPacketPayload(buffer, BUFFER_SIZE);;
    PacketHeader header = newHeader();

    header.payloadSize = sizeof(size_t);

    recv_packet(&sock, &header, &packet);

    printHeader(header);

    uint64_t value;
    memcpy(&value, packet.buffer, sizeof(uint64_t));
    value = ntohll(value);
    
    printf("Recebi a response: %ld\n", value );


    
    // printf(": %ld", );

    // int size = 1;

    // char buffer[1000];

    // while(1){

    //     int32_t rec = recv_buffer( &sock, (void*)buffer, 1000);
    
    //     if(rec < 0){
    //         fprintf(stderr, "Error in receive\n");
    //         //close socket and file .
    //         return -11;
    //     }

    // }



    fclose(dst);
    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
