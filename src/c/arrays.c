#include "../headers/arrays.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

StringArray create_string_array() {
    StringArray string = malloc(sizeof(struct StringArray));
    string->size = 0;
    string->array = malloc(sizeof(char *));
    return string;
}

void free_string_array(StringArray string) {
    if (string != NULL) {
        if(string->array != NULL){
            for (int i = 0; i < string->size; i++) {
                if (string->array[i] != NULL) {
                    free(string->array[i]);
                }
            }
            free(string->array);
        }
        free(string);
    }
}

StringArray string_to_array(char *string) {
    if (string == NULL) {
        return NULL;
    }
    StringArray stringArray = create_string_array();
    char *temp_string = malloc(strlen(string) + 1);
    strcpy(temp_string, string);

    char *token = strtok(temp_string, " ;");
    while (token != NULL) {
        stringArray->array = realloc(stringArray->array, sizeof(char *) * (stringArray->size + 1));
        stringArray->array[stringArray->size] = malloc(strlen(token) + 1);
        strcpy(stringArray->array[stringArray->size], token);

        stringArray->size++;
        token = strtok(NULL, " ;");
    }
    stringArray->array = realloc(stringArray->array, sizeof(char *) * (stringArray->size + 1));

    stringArray->array[stringArray->size] = malloc(strlen(END_OF_ARRAY) + 1);
    strcpy(stringArray->array[stringArray->size], END_OF_ARRAY); // null terminate the array.
    stringArray->size++;
    free(temp_string);
    return stringArray;
}
//char** string_to_array(char *string) {
//    if(string == NULL){
//        return NULL;
//    }
//
//    char* temp_string = malloc(strlen(string) + 1);
//    strcpy(temp_string, string);
//
//    int tokens = 0;
//    char **array = malloc(sizeof(char *) * 1);
//
//    char *token = strtok(temp_string, " ;");
//    while (token != NULL) {
//        array = realloc(array, sizeof(char *) * (tokens + 1));
//        array[tokens] = malloc(strlen(token) + 1);
//        strcpy(array[tokens], token);
//
//        tokens++;
//        token = strtok(NULL, " ;");
//    }
//    array = realloc(array, sizeof(char *) * (tokens + 1));
//
//    array[tokens] = malloc(strlen(END_OF_ARRAY) + 1);
//    strcpy(array[tokens], END_OF_ARRAY); // null terminate the array.
//
//    free(temp_string);
//    return array;
//}


int get_array_size(StringArray array) {
    return array->size - 1;
}

char *array_of_tokens_to_string(StringArray tokens, char *starting_token, char *ending_token, bool includes_starting_token) {
    char *string = NULL;
    size_t string_size = 0;
    int starting_index = 0;

    if (tokens == NULL || tokens->size == 0) {
        return NULL;
    }

    // get to the starting token.
    while (strcasecmp(tokens->array[starting_index], starting_token) != 0) {
        if (strcasecmp(tokens->array[starting_index], END_OF_ARRAY) == 0) { // starting token doesn't exist.
            return NULL;
        }
        starting_index++;
    }

    // calculate the total string size to create
    int index = starting_index;
    while (index < tokens->size && strcasecmp(tokens->array[index], ending_token) != 0) {
        if (strcasecmp(tokens->array[starting_index], END_OF_ARRAY) == 0) { // ending token doesn't exist.
            return NULL;
        }
        string_size += strlen(tokens->array[index]) + 1;
        index++;
    }

    // Build the string
    string = malloc(string_size);
    strcpy(string, "");
    int tmp_index = starting_index;

    // if we want the starting token, append it
    if (includes_starting_token) {
        strcat(string, tokens->array[tmp_index]);
        strcat(string, " ");
    }
    tmp_index++;

    // Build the rest of the clause
    strcat(string, tokens->array[tmp_index++]);
    for (int i = tmp_index; i < index; i++) {
        strcat(string, " ");
        strcat(string, tokens->array[i]);
    }

    return string;
}