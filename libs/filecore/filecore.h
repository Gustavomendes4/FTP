#ifndef FILECORE_INCLUDED
#define FILECORE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long int fileSize(FILE* file);

int isValidDirectory(const char* path);

int existsFile(const char* path);

int existsDirectory(const char* path);

#endif 