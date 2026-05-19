#ifndef TRANSFER_H_INCLUDED
#define TRANSFER_H_INCLUDED

#define CHUNCK_SIZE 4096

#include "minisocket.h"
#include "packet.h"
#include "packetlib/helpper.h"



int sendFile(ms_socket_t sock, Packet* packet, const char* filePath);

int recvFile(ms_socket_t sock, Packet* packet, const char* filePath);

int16_t sendFileInfos(ms_socket_t sock, Packet* packet, FILE* file);

int16_t recvFileInfos(ms_socket_t sock, Packet* packet, size_t* filesize, size_t* numChunks);

#endif //COMM_H_INCLUDED