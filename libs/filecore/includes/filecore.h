#ifndef FILECORE_INCLUDED
#define FILECORE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fileutils.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <errno.h>

    #include <dirent.h>
#endif


#define MAX_PATH_LENGTH 1000


long int fc_fileSize(FILE* file);

void fc_open(FILE** file, const char* path, const char* mode);

void fc_close(FILE* file);

int fc_delete(const char* path);

bool fc_createFolder(const char* path);

bool fc_deleteFolder(const char* path);

bool fc_isFolderEmpty(const char* path);

bool fc_move(const char* source, const char* destination);

bool fc_rename(const char* source, const char* destination);

bool fc_moveFolder(const char* oldpath, const char* newpath);

#endif 