/*



    Gustavo dos Santos Mendes, 25/03/2026.
*/


#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"


int toDigit(const char ch){

    if(!isDigit(ch))
        return -1;

    return ch - '0';
}

int toNumber(const char* arg){

    if( !isNumber(arg))
        return NaN;

    //...

    int i = 0, sign = 1, result = 0;

    // trata sinal
    if(arg[0] == '-'){
        sign = -1;
        i++;
    }
    else if(arg[0] == '+'){
        i++;
    }

    // converte dígitos
    for(i ; arg[i] != '\0'; i++){
        result = result * 10 + (arg[i] - '0');
    }

    return sign * result;
    //...
    return 5;
}

int isNumber(const char* vec){

    int i = 0;
    int decimal = 0;

    if(vec[0] == '-' || vec[0] == '+'){

        if( isDigit(vec[1]) )
            i = 2;

        else
            return 0;
    }


    for(i; vec[i] != '\0'; i++){
        
        if( !isDigit(vec[i]) ){

            if( (vec[i] == '.' || vec[i] == ',') && !decimal){
                decimal = 1;
            }
            else
                return 0;
        }
    }

    return 1;
}

int isDigit(const char ch){
    return (ch >= '0') && (ch <= '9');
}

// ======== RANDOM ===============
void initRandom() {
    srand((unsigned int)time(NULL));
}

int randomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

float randomFloat(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

// ===============================

int floatIsZero(float in){

    float absol = (in < 0) ? -(in) : in ;

    return ( absol < 0.0001 );

}

int contCharInStr(const char* str, char c) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == c) count++;
    }
    return count;
}

int getStrUntilChar(const char* str, char c, char* buffer, int bufferSize) {
    int i = 0;
    while (str[i] && str[i] != c && i < bufferSize - 1) {
        buffer[i] = str[i];
        i++;
    }
    buffer[i] = '\0';
    return i;
}

int isValidIp(const char* ip) {

    if(!ip || ip[0] == '\0') return 0;

    int len = strlen(ip);
    if( len > 15 || len < 7 ) return 0;

    if( contCharInStr(ip, '.') != 3 ) return 0;


    ///

    const char* ptr = ip;
    char octet[4];

    for(int segment = 0; segment < 4; segment++) {
        
        len = getStrUntilChar(ptr, '.', octet, sizeof(octet));
        
        if (len == 0 || len > 3) return 0;

        if (len == 3 && ptr[len] != '.' && ptr[len] != '\0') return 0;
        
        if( !isNumber(octet) ) return 0;
        
        int num = toNumber(octet);
        
        if (num > 255 || num < 0) return 0;
        
        ptr += len;
        
        if (segment < 3) {
            if (*ptr != '.') return 0;
            ptr++; // pula '.'
        }
    }

    return *ptr == '\0';
}

int isValidPort(const char* port) {
    
    if(!port || port[0] == '\0') return 0;

    if( !isNumber(port) ) return 0;

    int portNum = toNumber(port);

    return (portNum > 0 && portNum <= 65535);
}
