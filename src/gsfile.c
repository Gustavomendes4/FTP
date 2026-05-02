
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "gsfile.h"

size_t gs_fileSize(FILE* file){

    if(file == NULL)
        return 0;

    long pos = ftell(file);
    if(pos == -1L)
        return 0;

    if(fseek(file, 0, SEEK_END) != 0)
        return 0;

    long tam = ftell(file);
    if(tam == -1L)
        return 0;

    if(fseek(file, pos, SEEK_SET) != 0)
        return 0;

    return (size_t)tam;
}

