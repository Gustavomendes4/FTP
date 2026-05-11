#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define NaN -10001

#include <stdint.h>

int isNumber(const char*);

int isDigit(const char);

int toDigit(const char);

int toNumber(const char* arg);

int randomInt(int min, int max);

float randomFloat(float min, float max);

int floatIsZero(float);

int contCharInStr(const char* str, char c);

int getStrUntilChar(const char* str, char c, char* buffer, int bufferSize);

int isValidIp(const char* ip);

int isValidPort(const char* port);

//

uint64_t htonll(uint64_t v);

uint64_t ntohll(uint64_t v);


#endif