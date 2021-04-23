cmake_minimum_required(VERSION 3.17)
project(DBManagementSystem C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(src)
include_directories(src/headers)

add_executable(DBManagementSystem
        src/headers/arrays.h
        src/headers/attribute.h
        src/headers/catalog.h
        src/headers/clause_parser.h
        src/headers/database.h
        src/headers/ddl_parser.h
        src/headers/dml_manager.h
        src/headers/dml_parser.h
        src/headers/Enums.h
        src/headers/foreign_key.h
        src/headers/node.h
        src/headers/queue.h
        src/headers/select_parser.h
        src/headers/shunting_yard_parser.h
        src/headers/stack.h
        src/headers/storagemanager.h
        src/headers/table.h
        src/headers/tuple.h
        src/headers/type.h
        src/headers/unique.h
        src/headers/utils.h
        src/c/arrays.c
        src/c/attribute.c
        src/c/catalog.c
        src/c/clause_parser.c
        src/c/ddl_parser.c
        src/c/dml_manager.c
        src/c/dml_parser.c
        src/c/Enums.c
        src/c/foreign_key.c
        src/c/node.c
        src/c/queue.c
        src/c/select_parser.c
        src/c/shunting_yard_parser.c
        src/c/stack.c
        src/c/storagemanager.c
        src/c/table.c
        src/c/tuple.c
        src/c/type.c
        src/c/unique.c
        src/c/utils.c
        #src/drivers/test_expressions.c
        src/drivers/database.c)
