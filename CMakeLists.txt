cmake_minimum_required(VERSION 3.17)
project(DBManagementSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)
include_directories(src/headers)

add_executable(DBManagementSystem
        src/headers/storagemanager.h
        old/storagemanagerhelper.h
        old/lru-queue.h
        src/headers/dml_parser.h
        src/headers/ddl_parser.h
        src/headers/database.h
        old/storagemanager.c
        old/storagemanagerhelper.c
        old/lru-queue.c
        src/c/dml_parser.c
        src/c/ddl_parser.c
        src/drivers/database.c
        )
