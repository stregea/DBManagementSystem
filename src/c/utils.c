
#include "../headers/utils.h"
#include "../headers/Enums.h"
#include <string.h>
#include <stdio.h>

char *trimwhitespace( char *str ){
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void remove_spaces(char* str) {
    int count = 0;
  
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i];
    str[count] = '\0';
}

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

bool is_keyword( char * word ){
	return false;
}

bool is_data_type( char * t_str ){
	return true;
}

int get_data_type(char *value) {

    int i;
    double d;
    bool is_potentially_double = false;

    // hacky way to differenctiate ints from doubles by checking for a '.'
    for(int x = 0; x < strlen(value); x++){
        if(value[x] == '.'){
            is_potentially_double = true;
        }
    }

    if (!is_potentially_double && sscanf(value, "%d", &i) != 0) { // it's an integer
        return INTEGER;
    }
    if (is_potentially_double && sscanf(value, "%lf", &d) != 0) { // it's an float/double
        return DOUBLE;
    }
    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0) {
        return BOOL;
    }

    return CHAR; // return as a char/varchar.
}
