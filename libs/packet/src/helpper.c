
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

