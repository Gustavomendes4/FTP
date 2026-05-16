

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"
#include "utils.h"

#include "filecore.h"
#include "comm.h"

uint16_t sendBadRequest(ms_socket_t sock){

    Packet packet = newPacket(0);

    packet.header.type = MSG_BAD_REQUEST;

    uint16_t ret = sendPacket(sock, &packet);

    delPacket(&packet);

    return ret;
}

uint16_t sendBadRequest(ms_socket_t sock){

    Packet packet = newPacket(0);

    packet.header.type = MSG_BAD_REQUEST;

    uint16_t ret = sendPacket(sock, &packet);

    delPacket(&packet);

    return ret;
}




