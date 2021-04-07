#include "../headers/tuple.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void free_tuple(Tuple tuple){
    if(tuple != NULL){
        for(int i = 0; i < tuple->size; i++){
            if(tuple->tuple[i] != NULL){
                free(tuple->tuple[i]);
            }
        }
        free(tuple);
    }
}

Tuple create_tuple(char * tuple_string){
    Tuple tuple = NULL;
    bool is_in_string = false;

    int string_size = strlen(tuple_string);

    // insert '_' for each space that isn't within a string
    for(int i = 0; i < string_size; i++){
        if(tuple_string[i] == '"'){
            if(is_in_string){
                is_in_string = false;
            }else{
                is_in_string = true;
            }
        }else{
            if(tuple_string[i] == ' ' && is_in_string == false){
                tuple_string[i] = '_';
            }
        }
    }

    if(is_in_string == true){
        return NULL;
    }

    // create the tuple.
    tuple = malloc(sizeof(struct Tuple));
    tuple->size = 0;
    tuple->tuple = malloc(sizeof(char*));
    char *next_value = strtok(tuple_string, "_()");

    // store each attribute within an array in the tuple
    while(next_value != NULL){
        tuple->tuple = realloc(tuple->tuple, sizeof(char *) * (tuple->size + 1));

        tuple->tuple[tuple->size] = malloc(strlen(next_value) + 1);
        strcpy(tuple->tuple[tuple->size], next_value);
        tuple->size++;
        next_value = strtok(NULL, "_()");
    }

    return tuple;
}