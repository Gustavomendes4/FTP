
/*



*/

#include "filecore.h"
#include "fileutils.h"


int isValidPath(const char* path){

    return path != NULL;
}

int getFullPath(char* fullpath, const char* basepath){

    #ifdef _WIN32
        return GetFullPathNameA(basepath, MAX_PATH_LENGTH, fullpath, NULL) != 0;
    #else

        char* result = realpath(basepath, fullpath);
        return result != NULL;

    #endif
}

int pathBelongsToRoot(const char* path, const char* root){

    char completePath[MAX_PATH_LENGTH];
    char completeRoot[MAX_PATH_LENGTH];

    if(  !getFullPath(completePath, path)  ||  !getFullPath(completeRoot, root)  ){
        return 0;
    }

    int rootLen = strlen(completeRoot);

    char lastChar = completePath[rootLen];

    if( strncmp(completeRoot, completePath, rootLen) != 0 )
        return 0;

    if( !isPathSeparator(lastChar) )
        return 0;

    return 1;
}

int isPathSeparator(char ch){

    if(
        ch == '\0' ||
        ch == '/'
        
    #ifdef _WIN32
        || ch == '\\'
    #endif

    )   return 1;

    return 0;
}

int existsDirectory(const char* path){

    if( !path )
        return false;


    #ifdef _WIN32

    DWORD attr = GetFileAttributesA(path);

    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);

    #else

    struct stat info;

    if (stat(path, &info) != 0) {
        return 0;
    }

    return (info.st_mode & S_IFDIR) != 0;

    #endif
}

int existsFile(const char* path){

    struct stat info;

    if(path == NULL)
        return 0;

    if(stat(path, &info) != 0)
        return 0;

    return (info.st_mode & S_IFREG) != 0;
}

int buildPath( char* dst, size_t dstSize, const char* baseFolder, const char* relativePath ){

    if( dst == NULL || baseFolder == NULL || relativePath == NULL ) return 0;

    while(
        *relativePath == '/'
    #ifdef _WIN32
        || *relativePath == '\\' 
    #endif
    ){
        relativePath++;
    }

    size_t baseLen = strlen(baseFolder);

    int endsWithSeparator = 0;

    if(baseLen > 0){
        char last = baseFolder[baseLen - 1];

        if(last == '/' || last == '\\'){
            endsWithSeparator = 1;
        }
    }


    int written;

    if(endsWithSeparator)
        written = snprintf( dst, dstSize, "%s%s", baseFolder, relativePath );
    else
        written = snprintf( dst, dstSize, "%s/%s", baseFolder, relativePath );


    return ( written >= 0 && (size_t)written < dstSize );
}

const char* fc_getName(const char* path){

    if(path == NULL)
        return NULL;

    const char* slash1 = strrchr(path, '/');
    const char* slash2 = strrchr(path, '\\');

    const char* last = slash1;

    if(slash2 != NULL && (last == NULL || slash2 > last))
        last = slash2;

    return (last == NULL) ? path : last + 1;
}
