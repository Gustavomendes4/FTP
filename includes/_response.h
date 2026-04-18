#ifndef RESPONSE_H_INCLUDED
#define RESPONSE_H_INCLUDED

#include <stdint.h>

#define BlockMaxSize 1024

typedef struct FileResponseHeader {
    uint64_t fileSize;      // total size of the file being sent
    uint32_t dataSize;      // size of the current block of data being sent
    uint8_t code;           // indicates the status of the request (e.g., success, file not found, etc.)
} FileResponseHeader;

typedef struct FileResponse {
    FileResponseHeader header;
    uint8_t block[BlockMaxSize];
} FileResponse;



#endif // RESPONSE_H_INCLUDED