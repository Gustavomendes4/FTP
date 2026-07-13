
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "filecore.h"

long int fc_fileSize(FILE* file){

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

void fc_open(FILE** file, const char* path, const char* mode){
    
    *file = fopen(path, mode);
}

void fc_close(FILE* file){
    fclose(file);
}

int fc_delete(const char* path){
    
    if( !path )
        return 0;
    
    return remove(path);
}

bool fc_move(const char* source, const char* destination){

    if(source == NULL || destination == NULL)
        return false;

    if(!existsFile(source) && !existsDirectory(source))
        return false;

    if(!existsDirectory(destination))
        return false;

    char newPath[MAX_PATH_LENGTH];

    buildPath(
        newPath,
        MAX_PATH_LENGTH,
        destination,
        fc_getName(source)
    );

    return fc_rename(source, newPath);
}

bool fc_rename(const char* source, const char* destination){

    if(source == NULL || destination == NULL)
        return false;

    return rename(source, destination) == 0;
}

bool fc_createFolder(const char* path){

    if(path == NULL)
        return false;

#ifdef _WIN32

    if(CreateDirectoryA(path, NULL))
        return true;

    return GetLastError() == ERROR_ALREADY_EXISTS;

#else
    if(mkdir(path, 0777) == 0)
        return true;

    return errno == EEXIST;
#endif
}

bool fc_deleteFolder(const char* path){
    
    if(path == NULL)
        return false;
    
    #ifdef _WIN32
    
    return RemoveDirectoryA(path);
    
    #else
    
    return rmdir(path) == 0;

    #endif
}

bool fc_isFolderEmpty(const char* path){

    if(path == NULL)
        return false;

#ifdef _WIN32

    char searchPath[MAX_PATH];

    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);

    if(hFind == INVALID_HANDLE_VALUE)
        return false;

    do{

        if(strcmp(findData.cFileName, ".") != 0 &&
           strcmp(findData.cFileName, "..") != 0){

            FindClose(hFind);
            return false;
        }

    }while(FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return true;

#else

    DIR* dir = opendir(path);

    if(dir == NULL)
        return false;

    struct dirent* entry;

    while((entry = readdir(dir)) != NULL){

        if(strcmp(entry->d_name, ".") != 0 &&
           strcmp(entry->d_name, "..") != 0){

            closedir(dir);
            return false;
        }
    }

    closedir(dir);
    return true;

#endif
}

bool fc_moveFolder(const char* oldpath, const char* newpath){

    if( !oldpath || !newpath)
        return false;

    if( !existsDirectory(oldpath) || existsDirectory(newpath) )
        return false;

    if( pathBelongsToRoot(newpath, oldpath) )
        return false;

    return fc_rename(oldpath, newpath);
}