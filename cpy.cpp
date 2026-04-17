/*
    * Simple file copy program in C++.

    Ideia: biblioteca para manipulação, verificar se existe, copiar, validar path: arquivos.


    Gustavo dos Santos Mendes, 16/04/2026.
*/

#include <cstdio>
#include <cstring>

#if defined(_WIN32)

    
#else

    #include <unistd.h>
    // #include <fcntl.h>
    // #include<sys/stat.h> 
    // #include<sys/types.h>

    
#endif


int isValidPath(char* path) {
    return 1;
}

int main(int argc, char* argv[]){
    
    if(argc < 3){
        printf("Not enough arguments. Usage: cp <file_path> <new_file_path>\n");
        return -1;
    }

    if( !isValidPath(argv[1]) ){
        printf("%s is not a valid source file path.\n", argv[1]);
        return -2;
    }

    if( !isValidPath(argv[2]) ){
        printf("%s is not a valid destination file path.\n", argv[2]);
        return -3;
    }

    FILE* src = fopen(argv[1], "rb");
    
    if(src == NULL){
        printf("Could not open source file %s\n", argv[1]);
        return -4;
    }
    
    FILE* dst = fopen(argv[2], "wb");
    if(dst == NULL){
        printf("Could not open destination file %s\n", argv[2]);
        fclose(src);
        return -5;
    }


    while(!feof(src)){
        char buffer[1024];
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), src);
        if(bytesRead > 0){
            fwrite(buffer, 1, bytesRead, dst);
        }
    }

    printf("Copy completed successfully!\n");
    return 0;
}
