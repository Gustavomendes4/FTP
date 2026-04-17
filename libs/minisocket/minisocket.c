
#include "minisocket.h"
#include <string.h>

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
    
    */
}

ms_socket_t ms_socket_create_v6(){
    return socket(AF_INET6, SOCK_STREAM, 0);
}

void ms_close_socket(ms_socket_t sock){
    ms_close(sock);
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

ms_socket_t ms_accept(ms_socket_t server, char* ip, int* port){
    
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
int ms_send(ms_socket_t sock, const void* data, int size) {
    return send(sock, (const char*)data, size, 0);
}

int ms_recv(ms_socket_t sock, void* buffer, int size) {
    return recv(sock, (char*)buffer, size, 0);
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
int ms_isValidPath(char* path) {
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

// ================  Utils  ================
static int isDigit(char c) {
    return c >= '0' && c <= '9';
}

static int contCharInStr(const char* str, char c) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == c) count++;
    }
    return count;
}

static int getStrUntilChar(const char* str, char c, char* buffer, int bufferSize) {
    int i = 0;
    while (str[i] && str[i] != c && i < bufferSize - 1) {
        buffer[i] = str[i];
        i++;
    }
    buffer[i] = '\0';
    return i;
}

static int isNumber(const char* str) {
    if (!str || !*str) return 0;

    for (int i = 0; str[i]; i++) {
        if (!isDigit(str[i])) return 0;
    }
    return 1;
}

static int toNumber(const char* str) {
    int num = 0;
    for (int i = 0; str[i]; i++) {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}

// =======================================

int ms_isValidIp(const char* ip) {

    if(!ip || ip[0] == '\0') return 0;

    int len = strlen(ip);
    if( len > 15 || len < 7 ) return 0;

    if( contCharInStr(ip, '.') != 3 ) return 0;


    ///

    const char* ptr = ip;
    char octet[4];

    for(int segment = 0; segment < 4; segment++) {
        
        len = getStrUntilChar(ptr, '.', octet, sizeof(octet));
        
        if (len == 0 || len > 3) return 0;

        if (len == 3 && ptr[len] != '.' && ptr[len] != '\0') return 0;
        
        if( !isNumber(octet) ) return 0;
        
        int num = toNumber(octet);
        
        if (num > 255 || num < 0) return 0;
        
        ptr += len;
        
        if (segment < 3) {
            if (*ptr != '.') return 0;
            ptr++; // pula '.'
        }
    }

    return *ptr == '\0';
}
