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

static void hton_header(PacketHeader* head){
    head->magic       = htons(head->magic);
    head->version       = htons(head->version);
    
    head->type        = htonl(head->type);
    head->flags       = htonl(head->flags);
    head->payloadSize = htonll(head->payloadSize);
}

static void ntoh_header(PacketHeader* head){
    head->magic       = ntohs(head->magic);
    head->version     = ntohs(head->version);

    head->type        = ntohl(head->type);
    head->flags       = ntohl(head->flags);
    head->payloadSize = ntohll(head->payloadSize);
}

static int validateHeader(const PacketHeader* header){

    if( header->magic != PACKET_MAGIC_NUMBER )
        return -1;
        
    if( header->payloadSize > MAX_PAYLOAD_SIZE )
        return -2;

    return 0;
}

//================================================


//  ==========____SEND__/__RECIVE____==============

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


//  ===========____SEND_/_RECIVE -> BUFFER____===============

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


//  ===========______DEBUG______===============

void printHeader(PacketHeader head){
    
    printf("[Header] { magic: 0x%x | version: %d | type: %d | flags: %d | payloadSize: %lld } \n", \
    (int)head.magic, (int)head.version, (int)head.type, (int)head.flags, head.payloadSize);
    
}


//  ===========______HEADER______===============

PacketHeader newHeader(){
    return newHeaderType(MSG_DEFAULT);
}

PacketHeader newHeaderType(uint32_t type){
    PacketHeader header;

    header.version = PACKETSIZE_VERSION;
    header.magic = PACKET_MAGIC_NUMBER;
    
    header.type = type;
    
    header.flags = 0;
    header.payloadSize = 0;

    return header;
}


//  ===========______PACKET______===============

PacketPayload newPacketPayload(uint8_t* payload, uint64_t max){
    PacketPayload ret;

    ret.buffer = payload;
    ret.maxSize = max;

    return ret;
}



