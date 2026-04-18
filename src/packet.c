
#include <stdint.h>

#include "utils.h"
#include "packet.h"
#include "minisocket.h"

PacketHeader newHeader(){
    PacketHeader header;
    header.type = MSG_EMPYT;
    header.flags = 0;
    header.payloadSize = 0;
    header.magic = 0xBABE;

    return header;
}

Packet newPacket(uint8_t* payload, uint64_t max){
    Packet ret;

    ret.buffer = payload;
    ret.maxSize = max;

    return ret;
}

int validateHeader(PacketHeader* header){

    if( header->magic != 0xBABE )
        return -3;
        
    if( header->payloadSize > MAX_PAYLOAD_SIZE )
        return -2;

    return 0;
}

uint32_t send_packet(ms_socket_t* socket, Packet* payload, size_t payload_size){
    
}

uint32_t sys_send_packet(ms_socket_t* socket, Packet* payload, size_t payload_size){
    
}

// retorno: (> 0)  → bytes lidos (completo)     (0)    → conexão fechada    (-1)   → erro
int32_t sys_recv_packet(ms_socket_t* socket, PacketHeader* header, Packet* payload){

    int bytes_received = ms_recv_all(*socket, header, sizeof(PacketHeader));

    if (bytes_received == 0)
        return 0; // conexão fechada

    if (bytes_received < 0)
        return -1; // Error or connection closed

    // Converter campos do header de network byte order para host byte order
    header->type = ntohs(header->type);
    header->flags = ntohs(header->flags);
    header->payloadSize = ntohl(header->payloadSize);
    header->magic = ntohl(header->magic);


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

        if (bytes_received != (int)header->payloadSize)
            return -2;
    }
    
    return header->type; // Return message type
}

int32_t recv_packet(ms_socket_t* socket, uint8_t* payload, uint64_t payload_maxSize){

    Packet pck = newPacket(payload, payload_maxSize);

    PacketHeader head;

    return sys_recv_packet(socket, &head, &pck);

}
