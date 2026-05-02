#ifndef MS_FILE_INCLUDED
#define MS_FILE_INCLUDED

#include <stdio.h>
#include <stdlib.h>

typedef struct{

    FILE* file;

    size_t size;
    
}FileDesc;

size_t gs_fileSize(FILE* file);


#endif 