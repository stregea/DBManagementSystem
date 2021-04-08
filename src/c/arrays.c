#include "../headers/arrays.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void free_string_array(char **array) {
    if (array != NULL) {
        int index = 0;
        while (array[index] != NULL) {
            free(array[index++]);
        }
        free(array);
    }
}

char **string_to_array(char *string) {
//    int array_size = 0;
    int tokens = 0;
    char **array = malloc(sizeof(char *));

    char *token = strtok(string, " ;");
    while (token != NULL) {
        array = realloc(array, sizeof(char *) * (tokens + 1));
        array[tokens] = malloc(strlen(token) + 1);
        strcpy(array[tokens], token);

//        array_size++;
        tokens++;
        token = strtok(NULL, " ;");
    }
    array = realloc(array, sizeof(char *) * (tokens + 1));

    array[tokens] = malloc(strlen(END_OF_ARRAY) + 1);
    strcpy(array[tokens], END_OF_ARRAY); // null terminate the array.

    array[tokens + 1] = NULL;
    return array;
}


int get_array_size(char **array) {
    int size = 0;

    while (strcasecmp(array[size], END_OF_ARRAY) != 0) {
        size++;
    }

    return size;
}

char *array_of_tokens_to_string(char **tokens, char *starting_token, char *ending_token, bool includes_starting_token) {
    char *string = NULL;
    size_t string_size = 0;
    int starting_index = 0;

    // get to the starting token.
    while (strcasecmp(tokens[starting_index], starting_token) != 0) {
        if (tokens[starting_index] == NULL) { // starting token doesn't exist.
            return NULL;
        }
        starting_index++;
    }

    // calculate the total string size to create
    int index = starting_index;
    while (strcasecmp(tokens[index], ending_token) != 0) {
        if (tokens[index] == NULL) { // ending token doesn't exist.
            return NULL;
        }
        string_size += strlen(tokens[index]) + 1;
        index++;
    }

    // Build the string
    string = malloc(string_size);
    strcpy(string, "");
    int tmp_index = starting_index;

    // if we want the starting token, append it
    if (includes_starting_token) {
        strcat(string, tokens[tmp_index]);
        strcat(string, " ");
    }
    tmp_index++;

    // Build the rest of the clause
    strcat(string, tokens[tmp_index++]);
    for (int i = tmp_index; i < index; i++) {
        strcat(string, " ");
        strcat(string, tokens[i]);
    }

    return string;
}