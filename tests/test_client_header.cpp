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

// const char* ip = "10.34.57.213";
// const char* ip = "localhost";
const char* ip = "127.0.0.1";

const int port = 8000;

int create_socket_local(ms_socket_t* sock, const char* ip, int port){
    
    if(ms_init() != 0) {
        fprintf(stderr, "Could not initialize socket library\n");
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

    return 0;
}

int main(int argc, char* argv[]){

    ms_init();
    ms_socket_t sock;

    /// ========= Criação e conexão no socket    ========= ///
    if(create_socket_local(&sock, ip, port) != 0) {
        fprintf(stderr, "Socket creation or connection failed.\n");
        //close socket and file .
        return -10;
    }

    /// ========= | Envia soliciatação de arquivo |   ========= ///

    PacketPayload packet;
    PacketHeader header;

    char str[1000] = "Gustavo dos Santos Mendes";
    const int len = strlen(str);

    packet = newPacketPayload((uint8_t*)str, len);
    header = newHeaderType(MSG_GET_FILE);
    header.payloadSize = len;

    // Envia cabeçalho
    if( send_packet(&sock, &header, &packet) <= 0){
        fprintf(stderr, "Erro para enviar cabecalho\n");
         //close socket and file .
        return -11;
    }
    
    // Recebe resposta [Se válido, tamanho, ...]
    packet.maxSize = sizeof(str);

    int32_t ret = recv_packet(&sock, &header, &packet);

    if( ret <= 0){
        fprintf(stderr, "Erro para receber info [%d]\n", ret);
         //close socket and file .
        return -12;
    }

    // if       // Se valido
    //while()   // Recebe arquivo

    printHeader(header);

    printf("body: %s\n", packet.buffer );


    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
