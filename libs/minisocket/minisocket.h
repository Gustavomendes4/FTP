#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

// -lws2_32

// usar socket non-blocking para nao bloquear para sempre

#if defined(_WIN32)
    
    #include <ws2tcpip.h>
    #include <winsock2.h>
    #include <windows.h>

    typedef SOCKET ms_socket_t;

    #define ms_invalid INVALID_SOCKET

#else

    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <cerrno>



    typedef int ms_socket_t;
    #define ms_invalid -1

#endif



#endif // SOCKET_H_INCLUDED


// ================  Init / Cleanup  ================
int ms_init();
void ms_cleanup();
void ms_close(ms_socket_t sock);

// ================  Socket  ================
ms_socket_t ms_socket_create();
void ms_close_socket(ms_socket_t sock);

// ================  Client  ================
int ms_connect(ms_socket_t sock, const char* ip, int port);

// ================  Server  ================
int ms_bind(ms_socket_t sock, int port);
int ms_listen(ms_socket_t sock, int backlog);
ms_socket_t ms_accept(ms_socket_t server, char* ip, int* port);

// ================  Send / Recive  ================
int ms_send(ms_socket_t sock, const char* data, int size);
int ms_send_all(ms_socket_t sock, const void* buffer, int size);

int ms_recv(ms_socket_t sock, char* buffer, int size);
int ms_recv_all(ms_socket_t sock, void* buffer, size_t size);


// ================  Other  ================
// int ms_set_nonblocking(ms_socket_t sock);
int ms_last_error();

int ms_isValidIp(const char* ip);
int ms_isValidPath(const char* path);