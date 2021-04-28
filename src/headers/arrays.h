#ifndef DBMANAGEMENTSYSTEM_ARRAYS_H
#define DBMANAGEMENTSYSTEM_ARRAYS_H
#define END_OF_ARRAY "END_OF_ARRAY"

#include <stdbool.h>
struct StringArray{
    int size;
    char ** array;
}; typedef struct StringArray* StringArray;

StringArray create_string_array();

/**
 * create a string array of tokens.
 * @param string - The string to turn into an array.
 * @return an array of strings.
 */
StringArray string_to_array(char *string);

/**
 * Free a string array.
 * @param array - The array to free.
 */
void free_string_array(StringArray array);

/**
 * Calculate the size of the array;
 * @param array - The array to determine the size for;
 * @return the size of the array.
 */
int get_array_size(StringArray array);

/**
 * Create a string of tokens based on two specified tokens.
 * @param tokens - The array containing string tokens.
 * @param starting_token - The token to start at.
 * @param ending_token - The token to end at.
 * @param includes_starting_token - Boolean to determine to include or exclude the starting token within the string.
 * @return A string containing the tokens from the starting token to the ending token.
 */
char *array_of_tokens_to_string(StringArray tokens, char *starting_token, char *ending_token, bool includes_starting_token);

#endif
