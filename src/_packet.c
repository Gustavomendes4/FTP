/*

    O presente programa tem como objetivo o envio e recebimento 
    de pacotes em socket utilizando a lib minisocket.

    A ideia é implementar o envio e recebimento de pacotes:
        [HEADER]
        [ BODY ]

    Isso é feito através das funções sys_send_packet() e sys_recive_packet, 
    que enviam e recebem pacotes recebendo por parâmetro o header e o body, respectivamente.

    Acima disto, existe uma segunda camada de abstração, representada pelas funções: send_packe e recv_packet.
    Esta camada recebe apenas o conteúdo e o seu tamanho, enviando-o com um header padrão.


    Gustavo dos Santos Mendes, 22/04/2026.
*/

#include <stdint.h>
#include <stdio.h>

#include "utils.h"
#include "packet.h"
#include "minisocket.h"

int32_t send_packet(ms_socket_t* socket, PacketHeader* header, PacketPayload* payload){

    if (!socket || !header || !payload)
        return -99;

    PacketHeader net = *header;

    hton_header(&net);
    
    int bytes_sent = ms_send_all(*socket, &net, sizeof(PacketHeader));

    if(bytes_sent == 0)
        return -1;

    if(bytes_sent < 0)
        return -2;


    if(header->payloadSize > 0){

        bytes_sent = ms_send_all(*socket, payload->buffer, header->payloadSize);

        if(bytes_sent <= 0)
            return -3;
    }

    return header->type;
}

int32_t recv_packet(ms_socket_t* socket, PacketHeader* header, PacketPayload* payload){
    // retorno: (> 0)  → bytes lidos (completo)     (0)    → conexão fechada    (-1)   → erro

    int bytes_received = ms_recv_all(*socket, header, sizeof(PacketHeader));

    if (bytes_received == 0)
        return 0; // conexão fechada

    if (bytes_received < 0)
        return -1; // Error or connection closed

    ntoh_header(header);

    if(validateHeader(header) != 0)
        return -4; // Invalid header

    if (header->payloadSize > payload->maxSize)
        return -5; // Payload buffer too small


    if(header->payloadSize > 0){

        bytes_received = ms_recv_all(*socket, payload->buffer, header->payloadSize);
    
        if (bytes_received == 0)
            return 0; // Connection closed
    
        if (bytes_received < 0)
            return -1; // Error

        if ((uint64_t)bytes_received != header->payloadSize)
            return -2;
    }
    
    return header->type; // Return message type
}

int32_t recv_buffer(ms_socket_t* socket, void* payload, uint64_t payload_maxSize){

    PacketPayload pck = newPacketPayload((uint8_t*)payload, payload_maxSize);

    PacketHeader head;

    return recv_packet(socket, &head, &pck);
}

int32_t send_buffer(ms_socket_t* socket, void* payload, uint64_t payload_size){
    
    // if(payload_size > MAX_PAYLOAD_SIZE)
    //     return -10;

    PacketPayload pck = newPacketPayload( (uint8_t*)payload, payload_size);

    PacketHeader head = newHeader();
    head.payloadSize = payload_size;

    return send_packet(socket, &head, &pck);
}

// ============================

Packet pck_newPacket(size_t contBytes){

    Packet packet = {0};
    uint8_t* buffer;
    
    if(contBytes > 0){
        buffer = (uint8_t*)malloc(contBytes);

        if(buffer == NULL){
            fillErrorPacket(&packet);
            return packet; // error on allocation    
        }
    }
    else{
        buffer = NULL;
    }


    //  Create header    
    packet.header = newHeader();
    packet.header.payloadSize = contBytes;

    //  Create payload
    packet.payload = newPacketPayload(buffer, contBytes);

    return packet;
}

void pck_delPacket(Packet* pck){

    if(pck == NULL)
        return;

    free(pck->payload.buffer);

    *pck = (Packet){0};
}


int pck_isValidPacket(Packet pack){
    if(pack.header.magic != PACKET_MAGIC_NUMBER)
        return 0;

    if(pack.header.payloadSize > pack.payload.maxSize)
        return 0;

    return 1;
}

// Send / recv
int32_t pck_sendPacket(ms_socket_t* sock, Packet* packet){
    return send_packet(sock, &(packet->header), &(packet->payload));
}

int32_t pck_recvPacket(ms_socket_t* sock, Packet* packet){
    return recv_packet(sock, &(packet->header), &(packet->payload));
}

