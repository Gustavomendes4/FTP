#ifndef PACKET_HELPER_H_INCLUDED
#define PACKET_HELPER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "minisocket.h"
#include "packet.h"

uint16_t sendTypedHeader(ms_socket_t sock, uint32_t  type);






#endif