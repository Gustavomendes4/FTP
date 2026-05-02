
#include <string.h>

#include "minisocket.h"
#include "utils.h"

// ================  Init / Cleanup  ================
int ms_init() {
    #ifdef _WIN32
        WSADATA wsa;
        return WSAStartup(MAKEWORD(2,2), &wsa);
    #else
        return 0;
    #endif
}

void ms_cleanup() {
    #ifdef _WIN32
        WSACleanup();
    #endif
}

void ms_close(ms_socket_t sock){
#ifdef _WIN32
    shutdown(sock, SD_BOTH);
    closesocket(sock);
#else
    shutdown(sock, SHUT_RDWR);
    close(sock);
#endif
}

// ================  Socket  ================
ms_socket_t ms_socket_create(){
    return socket(AF_INET, SOCK_STREAM, 0);

    /*
        af | domain:
            AF_INET     // IPv4
            AF_INET6    // IPv6
            AF_UNIX     // Local socket (file system)
    
        type:
            SOCK_STREAM  // TCP
            SOCK_DGRAM   // UDP

            ...
            RAW_SOCKET  // Le rede completa (sudo)
    
    */
}

// ================  Client  ================
int ms_connect(ms_socket_t sock, const char* ip, int port){
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    addr.sin_addr.s_addr = inet_addr(ip);
    if (addr.sin_addr.s_addr == INADDR_NONE)
        return -1;

    return connect(sock, (struct sockaddr*)&addr, sizeof(addr));
}

// ================  Server  ================
int ms_bind(ms_socket_t sock, int port) {
    struct sockaddr_in addr;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    return bind(sock, (struct sockaddr*)&addr, sizeof(addr));
}

int ms_listen(ms_socket_t sock, int backlog){
    // Transforma socket em um socket de escuta, pronto para aceitar conexões
    return listen(sock, backlog);
}

int ms_initServer(ms_socket_t sock, int port, int backlog){

    if( ms_bind(sock, port) != 0)
        return -1; //error

    return ms_listen(sock, backlog);
}

ms_socket_t ms_accept(ms_socket_t server, char* ip, int* port){
    
    // ip = ip do client conectado
    // port = porta do client conectado

    struct sockaddr_in addr;

    #ifdef _WIN32
        int len = sizeof(addr);
    #else
        socklen_t len = sizeof(addr);
    #endif

    ms_socket_t client = accept(server, (struct sockaddr*)&addr, &len);

    if (client == ms_invalid)
        return ms_invalid;

    // Extrai IP
    if (ip) {
        #ifdef _WIN32
            // InetNtop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
            strcpy(ip, inet_ntoa(addr.sin_addr));
        #else
            inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
        #endif
    }

    // Extrai porta
    if (port) {
        *port = ntohs(addr.sin_port);
    }

    return client;
}

// ================  Send / Recive  ================
int ms_send(ms_socket_t sock, const char* buffer, int size) {
    return send(sock, buffer, size, 0);
}

int ms_send_all(ms_socket_t sock, const void* buffer, int size) {
    
    size_t total = 0;
    const char* ptr = (const char*)buffer;

    while (total < size) {
        int sent = ms_send(sock, ptr + total, (int)(size - total));

        if (sent <= 0){

            #ifndef _WIN32
            if (errno == EINTR) continue;
            #endif

            return -1;
        }

        total += (size_t)sent;
    }

    return (int)total;
}

int ms_recv(ms_socket_t sock, char* buffer, int size) {
    return recv(sock, buffer, size, 0);
}

int ms_recv_all(ms_socket_t sock, void* buffer, size_t size) {
    
    size_t total = 0;

    char* ptr = (char*)buffer;

    while(total < size) {
        int bytes_received = ms_recv(sock, ptr + total, size - total);
        

        if (bytes_received < 0) {
        #ifndef _WIN32
            if (errno == EINTR) continue;
        #endif
            return -1;
        }

        total += (size_t)bytes_received;
    }
    
    return (int)total;

}

// ================  Other  ================

int ms_last_error() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}


// ===================================
int ms_isValidPath( const char* path) {
    if (!path || !*path) return 0;

    #ifdef _WIN32
    
    const char* invalid = "<>:\"|?*";
    
    while (*path) {
        if (strchr(invalid, *path)) return 0;
        path++;
    }
    
    #endif

    return 1;
}

// =======================================

int ms_isValidIp(const char* ip) {
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));

    #ifdef _WIN32                
        int sa_len = sizeof(sa);
        int result = WSAStringToAddressA((LPSTR)ip, AF_INET, NULL, (LPSOCKADDR)&sa, &sa_len);
        return result == 0;

    #else
        return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
    #endif

}   

