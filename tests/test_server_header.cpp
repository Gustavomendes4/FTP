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

int main(int argc, char* argv[]){
    
    ms_socket_t sock;

    ms_init();

    if( (sock = ms_socket_create()) == ms_invalid){
        fprintf(stderr, "Could not create socket.\n");
        return -5;
    }

    if( ( ms_initServer(sock, 8000, 5)) == ms_invalid){
        fprintf(stderr, "Error to create server.\n");
        return -5;
    }

    
    printf("Server waiting...\n");
    ms_socket_t s1 = ms_accept(sock, NULL, NULL);


    /// ========= | Envia soliciatação de arquivo |   ========= ///


    char buffer[1000];
    PacketPayload packet = newPacketPayload((uint8_t*)buffer, 1000);
    PacketHeader header;

    // recebe solicitação
    if( recv_packet(&s1, &header, &packet) <= 0){
        fprintf(stderr, "Erro para receber info\n");
         //close socket and file .
        return -12;
    }

    printf("Solicitacao recebido: \n");
    printHeader(header);
    
    printf("\nbody:\n %s\n", packet.buffer);
    
    // Responde
    strcpy(buffer, "Recebi! Me chamo Gustavo Mendes. =)");
    
    header = newHeaderType(MSG_ACK);
    header.payloadSize = strlen(buffer) + 1;
    
    packet = newPacketPayload((uint8_t*)buffer, strlen(buffer) + 1);

    printf("\nTentar enviar...\n");
    if( send_packet(&s1, &header, &packet) <= 0){
        fprintf(stderr, "Erro para enviar info\n");
         //close socket and file .
        return -12;
    }


    ms_close(s1);
    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
