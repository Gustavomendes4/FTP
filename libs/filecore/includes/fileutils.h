#ifndef FILEUTILS_H_INCLUDED
#define FILEUTILS_H_INCLUDED

#ifdef _WIN32
    #include <windows.h>
    #include <sys/stat.h>

    #define stat _stat
#else
    #include <sys/stat.h>
#endif


int getFullPath(char* fullpath, const char* basepath);

int pathBelongsToRoot(const char* path, const char* root);

int isPathSeparator(char ch);

int isValidPath(const char* path);

int existsFile(const char* path);

int existsDirectory(const char* path);

int buildPath(char* dst, size_t dstSize, const char* baseFolder, const char* relativePath);

const char* fc_getName(const char* path);

#endif