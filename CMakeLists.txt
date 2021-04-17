cmake_minimum_required(VERSION 3.17)
project(DBManagementSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)
include_directories(src/headers)

add_executable(DBManagementSystem
        src/headers/storagemanager.h
        src/headers/dml_parser.h
        src/headers/ddl_parser.h
        src/headers/database.h
        src/c/storagemanager.c
        src/c/dml_parser.c
        src/c/ddl_parser.c
        src/drivers/database.c
        #src/drivers/test_expressions.c
        src/c/ddl_manager.c
        src/headers/ddl_manager.h
        src/c/dml_manager.c
        src/headers/dml_manager.h
        src/c/select_parser.c
        src/headers/select_parser.h
        src/c/Enums.c
        src/headers/Enums.h
        src/c/catalog.c
        src/headers/catalog.h
        src/c/table.c
        src/headers/table.h
        src/c/clause_parser.c
        src/headers/clause_parser.h
        src/c/arrays.c
        src/headers/arrays.h
        src/c/tuple.c
        src/headers/tuple.h
        src/c/node.c
        src/headers/node.h
        src/c/queue.c
        src/headers/queue.h
        src/c/stack.c
        src/headers/stack.h
        src/c/shunting_yard_parser.c
        src/headers/shunting_yard_parser.h)
