cmake_minimum_required(VERSION 3.10)
project(chip8-emulator LANGUAGES C)

set(EXECUTABLE_NAME chip8-emulator)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED)

add_executable(${EXECUTABLE_NAME} main.c chip8.c)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE SDL2::SDL2)
