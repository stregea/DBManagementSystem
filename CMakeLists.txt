cmake_minimum_required(VERSION 3.17)
project(DBManagementSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)
include_directories(src/headers)

add_executable(DBManagementSystem
        src/headers/storagemanager.h src/c/storagemanager.c src/phases/phase1.c)
