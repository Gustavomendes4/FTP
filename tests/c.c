#include <stdio.h>
#include <stdlib.h>
#include "minisocket.h"
#include "utils.h"

int main() {
    if (ms_init() != 0) {
        fprintf(stderr, "Failed to initialize socket library\n");
        return EXIT_FAILURE;
    }

    ms_socket_t sock = ms_socket_create();

    if (sock == ms_invalid) {
        fprintf(stderr, "Failed to create socket\n");
        ms_cleanup();
        return EXIT_FAILURE;
    }
    printf("Socket created successfully\n");


    ms_close_socket(sock);
    ms_cleanup();
    return EXIT_SUCCESS;
}