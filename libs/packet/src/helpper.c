
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"


uint16_t sendTypedHeader(ms_socket_t sock, uint32_t  type){

    PacketHeader head = newPacketHeaderType(type);

    return sendPacketHeader(sock, &head);
}

uint16_t sendPacketString(ms_socket_t sock, uint32_t type, const char* str){

    uint16_t ret;

    size_t len = strlen(str);

    // Header
    PacketHeader head = newPacketHeaderType(type);

    head.payloadSize = len;

    // Payload
    PacketPayload payload = newPacketPayload( (uint8_t*)str, len);


    ret = sendPacketHeader(sock, &head);
    
    if( ret != 0) return ret;

    return sendPacketPayload(sock, &payload, len);
}

uint64_t packetReadStr(Packet* packet, uint8_t* buffer, size_t maxSize){

    if(packet == NULL || buffer == NULL || maxSize == 0)
        return 0;

    if(isValidPacket(packet) != 0)
        return 0;

    if(packet->cursor >= packet->header.payloadSize){
        buffer[0] = '\0';
        return 0;
    }

    uint64_t readed = 0;

    while(packet->cursor < packet->header.payloadSize && readed < (maxSize - 1)){

        uint8_t c = packet->payload.buffer[packet->cursor++];

        buffer[readed++] = c;

        if(c == '\0'){
            return readed - 1;
        }
    }

    buffer[readed] = '\0';

    return readed;
}


//
uint64_t packetWriteString(Packet* packet, uint32_t type, const char* str){

    if( packet == NULL) return 0;

    // clearPacket(packet);
    setPacketType(packet, type);

    return  packetWrite(packet, str, strlen(str) + 1);
}

uint32_t getPacketType(Packet* packet){
    if(packet == NULL)
        return 0;

    return packet->header.type;
}
