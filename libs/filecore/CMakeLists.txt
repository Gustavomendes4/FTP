cmake_minimum_required(VERSION 3.10)

project(filecore LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Cria biblioteca estática
add_library(filecore STATIC
    src/filecore.c
    src/fileutils.c
)

# Diretório dos headers
target_include_directories(filecore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# Flags de compilação
target_compile_options(filecore PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

