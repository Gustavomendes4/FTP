
#include <stdint.h>

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

//========================

PacketHeader newHeader(){
    PacketHeader header;
    header.type = MSG_EMPTY;
    header.version = 1;
    header.flags = 0;
    header.payloadSize = 0;
    header.magic = PACKET_MAGIC_NUMBER;

    return header;
}

Packet newPacket(uint8_t* payload, uint64_t max){
    Packet ret;

    ret.buffer = payload;
    ret.maxSize = max;

    return ret;
}

int validateHeader(const PacketHeader* header){

    if( header->magic != PACKET_MAGIC_NUMBER )
        return -1;
        
    if( header->payloadSize > MAX_PAYLOAD_SIZE )
        return -2;

    return 0;
}

int32_t sys_send_packet(ms_socket_t* socket, PacketHeader* header, Packet* payload){

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

int32_t send_packet(ms_socket_t* socket, void* payload, uint64_t payload_size){
    
    // if(payload_size > MAX_PAYLOAD_SIZE)
    //     return -10;

    Packet pck = newPacket( (uint8_t*)payload, payload_size);

    PacketHeader head = newHeader();
    head.payloadSize = payload_size;

    return sys_send_packet(socket, &head, &pck);
}

// retorno: (> 0)  → bytes lidos (completo)     (0)    → conexão fechada    (-1)   → erro
int32_t sys_recv_packet(ms_socket_t* socket, PacketHeader* header, Packet* payload){

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

int32_t recv_packet(ms_socket_t* socket, void* payload, uint64_t payload_maxSize){

    Packet pck = newPacket((uint8_t*)payload, payload_maxSize);

    PacketHeader head;

    return sys_recv_packet(socket, &head, &pck);

}
