/*
    * Simple file copy program in C++.

    Ideia: biblioteca para manipulação, verificar se existe, copiar, validar path: arquivos.


    Gustavo dos Santos Mendes, 16/04/2026.

    argv[0]: -.exe
    argv[1]: <port>

//  Next step:

    argv[0]: server.exe

    argv[1]: <port>

    argv[2]: <base_folder>

*/

#include <cstdio>
#include <cstring>
#include <cstdint>

#include "minisocket.h"
#include "packet.h"
#include "gsfile.h"

#include "utils.h"

#define MAX_PATH_SIZE 1024

int main(int argc, char* argv[]){
    
    /// ========= Validação de argumentos     ========= ///
    if(argc < 3 || !isNumber(argv[1])){
        fprintf(stderr, "Invalid input. Use: <-.exe> <port> <dir>");
        return -1;
    }

    const int port = toNumber(argv[1]);

    /// ========= Criação de socket   ========= ///

    ms_init();

    ms_socket_t sock = ms_socket_create();
    
    if(sock == ms_invalid){
        fprintf(stderr, "Error to create socket.");
        return -2;
    }

    if( ms_initServer(sock, port, 5) < 0){
        fprintf(stderr, "Error to create server on socket.");
        return -2;
    }

    printf("Server initialized.\nWaiting connection...\n");

    int client_port;
    char client_ip[40];

    ms_socket_t s1 = ms_accept(sock, client_ip, &client_port);

    if(s1 == ms_invalid){
        fprintf(stderr, "Accept failed.\n");
        return -3;
    }

    printf("Client connected: (%s:%d)\n\n", client_ip, client_port);

    //  ======  Recebimento da mensagem ==========

    uint8_t buffer[2000];
    PacketPayload payload = newPacketPayload(buffer, 2000);
    PacketHeader header = newHeader();
    
    int32_t ret = recv_packet(&s1, &header, &payload);
    
    /*
    Packet pack = pck_newPacket(1200);
    
    int32_t ret = pck_recvPacket(&s1, &pack);
    */
    



    if(ret < 0){
        fprintf(stderr, "Error to recive packet[%d]", (int)ret);
        return -2;
    }

    printHeader(header);

    printf("body: %.*s\n", (int)header.payloadSize, payload.buffer);


    //  ======  Validação do arquivo ==========
    PacketHeader headerR;
    PacketPayload payR;

    char complete_path[MAX_PATH_SIZE] = "./files_testes/";

    strcat(complete_path, (char*)(payload.buffer));

    printf("\nallfile: %s\n", complete_path);

    FILE* f = fopen(complete_path, "rb");

    if(f == NULL){
        
        //  Envia arquivo not found
        headerR = newHeaderType(MSG_ERROR_FILE_NOT_FOUND);
        payR = newPacketPayload(NULL, 0);
        send_packet(&sock, &headerR, &payR);

        printf(": Arquivo nao existente.\n");

        return -6;
    }

    //  Ler tamanho do arquivo
    
    size_t file_tam = gs_fileSize(f);

    headerR = newHeaderType(MSG_FILE_FOUND);
    headerR.payloadSize = sizeof(size_t);

    payR.buffer = (uint8_t*)&file_tam;

    send_packet(&s1, &headerR, &payR);

    printf("Enviado response: \n");
    printHeader(headerR);
    printf("\n%lu", *payR.buffer);
    // Envia resposta ok

/*

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
    // #define BUFFER_SIZE 1450
    // uint8_t buffer[BUFFER_SIZE];

    PacketHeader headSend = newHeaderType(MSG_GET_FILE);
    headSend.payloadSize = strlen(argv[3]) + 1;

    PacketPayload packetSend = newPacketPayload((uint8_t*)argv[3], strlen(argv[3])+1);
    
    ret = send_packet(&sock, &headSend, &packetSend);

    if(ret < 0){
        fprintf(stderr, "Error to send path\n");
        //close socket and file .
        return -11;
    }



    // PacketPayload packet;
    // PacketHeader header;



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
*/
    ms_close(s1);
    ms_close(sock);
    ms_cleanup();

    printf("Copy completed successfully!\n");
    return 0;
}
