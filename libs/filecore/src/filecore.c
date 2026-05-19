
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filecore.h"

long int fc_fileSize(FILE* file){

    if(file == NULL) return 0;
    
    long curr = ftell(file);

    if(curr < 0) return 0;


    if(fseek(file, 0, SEEK_END) != 0)
        return 0;

    long end = ftell(file);

    if(end < 0) return 0;

    // Restaura posição original
    if(fseek(file, curr, SEEK_SET) != 0)
        return -1;

    return end;
}

void fc_open(FILE** file, const char* path, const char* mode){
    
    *file = fopen(path, mode);
}

void fc_close(FILE* file){
    fclose(file);
}

int fc_delete(const char* path){
    return 1;
}