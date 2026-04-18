/*



    Gustavo dos Santos Mendes, 25/03/2026.
*/


#include <stdlib.h>
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

