#include "../headers/arrays.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void free_string_array(char** array){
    if(array != NULL){
        int index = 0;
        while(array[index] != NULL){
            free(array[index++]);
        }
        free(array);
    }
}

char ** string_to_array(char* string){
    int array_size = 0;
    int tokens = 0;
    char ** array = malloc(sizeof(char*));

    char*token = strtok(string, " ;");
    while(token != NULL){
        array = realloc(array, sizeof(char*) * ++array_size);
        array[tokens] = malloc(strlen(token)+1);
        strcpy(array[tokens], token);

        tokens++;
        token = strtok(NULL, " ;");
    }
    array = realloc(array, sizeof(char*) * ++array_size);

    array[tokens] = malloc(strlen(END_OF_ARRAY)+1);
    strcpy(array[tokens++], END_OF_ARRAY); // null terminate the array.

    array[tokens] = NULL;
    return array;
}

char * array_of_tokens_to_string(char **tokens, char* starting_token, char* ending_token){
    char* string = NULL;
    size_t string_size = 0;
    int starting_index = 0;

    // get to the starting token.
    while(strcasecmp(tokens[starting_index], starting_token) != 0){
        if(tokens[starting_index] == NULL){ // starting token doesn't exist.
            return NULL;
        }
        starting_index++;
    }

    // calculate the total string size to create
    int index = starting_index;
    while(strcasecmp(tokens[index], ending_token) != 0){
        if(tokens[index] == NULL){ // ending token doesn't exist.
            return NULL;
        }
        string_size += strlen(tokens[index]) + 1;
        index++;
    }

    // Build the string
    string = malloc(string_size);
    strcpy(string, tokens[starting_index]);
    for(int i = starting_index+1; i < index;i++){
        strcat(string, " ");
        strcat(string, tokens[i]);
    }

    return string;
}