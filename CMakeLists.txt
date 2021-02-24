cmake_minimum_required(VERSION 3.17)
project(DBManagementSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)
include_directories(src/headers)

add_executable(DBManagementSystem
        src/headers/storagemanager.h
        src/headers/storagemanagerhelper.h
        src/headers/lru-queue.h
        src/headers/dml_parser.h
        src/headers/ddl_parser.h
        src/headers/database.h
        src/c/storagemanager.c
        src/c/storagemanagerhelper.c
        src/c/lru-queue.c
        src/c/dml_parser.c
        src/c/ddl_parser.c
        src/c/database.c
        src/drivers/phase1.c)
