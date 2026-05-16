
cmake_minimum_required(VERSION 3.10)

project(FTP LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Adiciona libs
add_subdirectory(libs/minisocket)
add_subdirectory(libs/packet)
add_subdirectory(libs/filecore)
add_subdirectory(libs/utils)

# Executável client
add_executable(client
    src/client.c

)

# Executável server
add_executable(server
    src/server.c
    src/serverHandlers.c

)

# Includes
target_include_directories(client PRIVATE
    includes
)

target_include_directories(server PRIVATE
    includes
)

# Linkagem
target_link_libraries(client PRIVATE
    packet
    minisocket
    utils
    filecore
)

target_link_libraries(server PRIVATE
    packet
    minisocket
    utils
    filecore
)