cmake_minimum_required(VERSION 3.10)

project(minisocket LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Cria biblioteca estática
add_library(minisocket STATIC
    minisocket.c
)

# Diretório dos headers
target_include_directories(minisocket PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Flags de compilação
target_compile_options(minisocket PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)