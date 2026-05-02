
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char* arguments[] = {
    "a",
    "ms",
    "utils",
    "packet",

    "server",
    "client",

    "\0"
};

const char* compile_cmd[] = {
    "cd ./bin && ar rcs lib.a minisocket.o packet.o utils.o packet.o",
    "g++ ./libs/minisocket/minisocket.c -I./includes -I./libs/utils -c -o ./bin/minisocket.o",
    "g++ ./libs/utils/utils.c -I./libs/utils -c -o ./bin/utils.o",
    "g++ ./src/packet.c -I./libs/utils -I./libs/minisocket -I./includes -c -o ./bin/packet.o",

    "g++ server.cpp ./src/gsfile.c -I./includes -I./libs/minisocket -I./libs/utils ./bin/lib.a -lws2_32 -o server"
    "g++ client.cpp ./src/gsfile.c -I./includes -I./libs/minisocket -I./libs/utils ./bin/lib.a -lws2_32 -o client"

    "\0"
};

int getIndex(const char* str){

    for(int i = 0; arguments[i][0] != '\0'; i++){

        // printf("strcmp(%s, %s) == %d", str, arguments[i], strcmp(str, arguments[i]));
        if( strcmp(str, arguments[i]) == 0 )
            return i;

    }

    return -1;
}

int compile(const char* str){

    // printf("%s", str);
    return system(str);
}

int main(int argc, char* argv[]){

    if(argc < 2){
        fprintf(stderr, "Not enough arguments\n");
        return -1;
    }


    for(int i = 1; i < argc; i++){
        
        int idx = getIndex(argv[i]);
    
        if(idx == -1)
            printf("Option '%s' is not a valid compilation goal\n", argv[i]);
        
        else{
            printf("Compiling [%s]... ", argv[i]);

            int ret = compile(compile_cmd[idx]);

            if(ret != 0)
                printf("Error(%d)!\n", ret);
            else
                printf("Done!\n");
            
        }
    }


    // printf(": '%s' -> %d", argv[1], idx);
    putchar('\n');
    return 0;
}
