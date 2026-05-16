
#include <string.h>

#include "filecore.h"

long int fileSize(FILE* file){

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

int isValidDirectory(const char* path){
    return 1;
}

int existsFile(const char* path){
    return 1;
}

int existsDirectory(const char* path){
    return 1;
}


