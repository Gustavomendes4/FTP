cmake_minimum_required(VERSION 3.10)

project(utils LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Cria biblioteca estática
add_library(utils STATIC
    utils.c

)

# Diretório dos headers
target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Flags de compilação
target_compile_options(utils PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)