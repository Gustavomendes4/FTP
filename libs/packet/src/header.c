/*
*
*
*
*
*
*
*
*
*
*
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// #include "includes/header.h"
#include "header.h"

// C:\Users\Gustavo\Desktop\FTP\libs\packet>g++ ./src/header.c -c -I./includes -I../minisocket

//  =========   STATIC FUNCTIONS    ==============

static uint64_t htonll(uint64_t value) {

    static const int num = 42;

    // little endian
    if (*(const char*)&num == 42) {
        return ((uint64_t)htonl((uint32_t)(value & 0xFFFFFFFFULL)) << 32) |
                htonl((uint32_t)(value >> 32));
    }

    // big endian
    return value;
}

static uint64_t ntohll(uint64_t value) {
    return htonll(value);
}

static size_t serializeHeaderBuffer(uint8_t* buffer, const PacketHeader* head){

    if(buffer == NULL || head == NULL) return 0;

    size_t idx = 0;

    uint16_t magic = htons(head->magic);
    memcpy(buffer + idx, &magic, sizeof(magic));
    idx += sizeof(magic);

    uint16_t version = htons(head->version);
    memcpy(buffer + idx, &version, sizeof(version));
    idx += sizeof(version);

    uint32_t type = htonl(head->type);
    memcpy(buffer + idx, &type, sizeof(type));
    idx += sizeof(type);

    uint32_t flags = htonl(head->flags);
    memcpy(buffer + idx, &flags, sizeof(flags));
    idx += sizeof(flags);

    uint64_t payloadSize = htonll(head->payloadSize);
    memcpy(buffer + idx, &payloadSize, sizeof(payloadSize));
    idx += sizeof(payloadSize);

    return idx;
}

static size_t parseHeaderBuffer(const uint8_t* buffer, PacketHeader* head){

    if(buffer == NULL || head == NULL)
        return 0;

    size_t idx = 0;

    uint16_t magic;
    memcpy(&magic, buffer + idx, sizeof(magic));
    head->magic = ntohs(magic);
    idx += sizeof(magic);

    uint16_t version;
    memcpy(&version, buffer + idx, sizeof(version));
    head->version = ntohs(version);
    idx += sizeof(version);

    uint32_t type;
    memcpy(&type, buffer + idx, sizeof(type));
    head->type = ntohl(type);
    idx += sizeof(type);

    uint32_t flags;
    memcpy(&flags, buffer + idx, sizeof(flags));
    head->flags = ntohl(flags);
    idx += sizeof(flags);

    uint64_t payloadSize;
    memcpy(&payloadSize, buffer + idx, sizeof(payloadSize));
    head->payloadSize = ntohll(payloadSize);
    idx += sizeof(payloadSize);

    return idx;
}

//  ==============================================

int validatePacketHeader(const PacketHeader* header){

    if(header == NULL) return -1;

    if( header->magic != PACKET_MAGIC_NUMBER )
        return -2;
        
    // if( header->payloadSize > MAX_PAYLOAD_SIZE )
    //     return -3;

    if(header->version != PACKET_CURR_VERSION)
        return -4;

    return 0;
}

void printHeader(PacketHeader head){
    
    printf("[Header] { magic: 0x%x | version: %d | type: %d | flags: %d | payloadSize: %lld } \n", \
    (int)head.magic, (int)head.version, (int)head.type, (int)head.flags, (long long int)head.payloadSize);
    
}

PacketHeader newPacketHeader(){
    return newPacketHeaderType(MSG_DEFAULT);
}

PacketHeader newPacketHeaderType(uint32_t type){
    PacketHeader header;

    header.version = PACKET_CURR_VERSION;
    header.magic = PACKET_MAGIC_NUMBER;
    
    header.type = type;
    
    header.flags = 0;
    header.payloadSize = 0;

    return header;
}


int16_t sendPacketHeader(ms_socket_t sock, const PacketHeader* header){

    if(header == NULL) return -1;

    uint8_t buffer[ PACKET_HEADER_SIZE ];

    size_t size = serializeHeaderBuffer(buffer, header);

    if(size != PACKET_HEADER_SIZE) return -20;

    int bytes_sent = ms_send_all(sock, buffer, size);

    if(bytes_sent == 0) return -1;

    if(bytes_sent <  0) return -4;


    return 0;
}

int16_t recvPacketHeader(ms_socket_t sock, PacketHeader* header){

    if(header == NULL) return -1;

    uint8_t buffer[ PACKET_HEADER_SIZE ];

    int bytes_received = ms_recv_all(sock, buffer, PACKET_HEADER_SIZE);

    if(bytes_received == 0) return -2;

    if(bytes_received <  0) return -3;
    

    parseHeaderBuffer(buffer, header);

    if(validatePacketHeader(header) < 0)
        return -4;
    
    return 0;
}
