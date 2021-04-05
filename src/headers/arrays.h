#ifndef DBMANAGEMENTSYSTEM_ARRAYS_H
#define DBMANAGEMENTSYSTEM_ARRAYS_H
#define END_OF_ARRAY "END_OF_ARRAY"

/**
 * create a string array of tokens.
 * @param string - The string to turn into an array.
 * @return an array of strings.
 */
char **string_to_array(char *string);

/**
 * Free a string array.
 * @param array - The array to free.
 */
void free_string_array(char **array);

/**
 * Create a string of tokens based on two specified tokens.
 * @param tokens
 * @param starting_token
 * @param ending_token
 * @return
 */
char * array_of_tokens_to_string(char **tokens, char* starting_token, char* ending_token);
#endif
