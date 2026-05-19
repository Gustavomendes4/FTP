#ifndef FILECORE_INCLUDED
#define FILECORE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"

#define MAX_PATH_LENGTH 1000


long int fc_fileSize(FILE* file);

void fc_open(FILE** file, const char* path, const char* mode);

void fc_close(FILE* file);

int fc_delete(const char* path);

#endif 