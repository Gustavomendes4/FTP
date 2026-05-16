cmake_minimum_required(VERSION 3.10)

project(packet LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Cria a biblioteca estática
add_library(packet STATIC
    src/header.c
    src/packet.c
    src/payload.c

    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/minisocket/minisocket.o

    src/helpper.c
)

# Diretórios de include da lib
target_include_directories(packet
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/includes

        ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/minisocket/
)

target_compile_options(packet PRIVATE
    -Wall
    -Wextra
    -Wpedantic

    -Wpedantic
)