#ifndef REQUEST_H_INCLUDED
#define REQUEST_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "minisocket.h"


#define MAX_PATH_SIZE 512


typedef struct {
    uint32_t type;
    uint32_t pathSize;

} FileRequestHeader;


typedef struct FileRequest{
    FileRequestHeader header;
    uint8_t filePath[MAX_PATH_SIZE];     // path of the requested file
    
} FileRequest;

#define FileRequestSize sizeof(FileRequest)




FileRequestHeader newFileRequestHeader(RequestType type, size_t pathSize) {
    FileRequestHeader header;
    header.type = type;
    header.pathSize = pathSize;
    return header;
}

FileRequest newFileRequest(FileRequestHeader head, const char* path) {
    FileRequest request;
    request.header = head;

    memset(request.filePath, 0, MAX_PATH_SIZE);

    for(int i = 0; i < head.pathSize; i++) {
        request.filePath[i] = path[i];
    }
    
    return request;
}

FileRequest createFileRequest(RequestType type, const char* path, size_t pathSize) {
    FileRequestHeader head = newFileRequestHeader(type, pathSize);
    return newFileRequest(head, path);
}

int RequestFile(const char* path, const char* dstPath, ms_socket_t socket ) {

    //Validar caminhos
    if( !ms_isValidPath(path) || !ms_isValidPath(dstPath) ) {
        fprintf(stderr, "Invalid file path(s).\n");
        return -1;
    }

    // ======  Abrir arquivo de dstino para escrita (verificar se é possível criar o arquivo)  ======
    FILE* dstFile = fopen(dstPath, "wb");
    
    if (!dstFile) {
        fprintf(stderr, "Could not open destination file %s\n", dstPath);
        return -2;
    }
    
    // ======  Montar a estrutura de requisição  ====== 
    FileRequest request = createFileRequest(REQ_GET_FILE, path, strlen(path));

    //  ======  Enviar a requisição  ===
    ms_send(socket, &request, FileRequestSize);


    FileResponse response;
    //loop:    
        //Receber a resposta
        ms_recv(socket, &response, sizeof(response));

        //Escrever no arquivo de destino


    //Fechar o socket

    //



}




#endif // RESPONSE_H_INCLUDED