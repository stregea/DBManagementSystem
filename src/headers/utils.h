
#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <ctype.h>

char *trimwhitespace( char *str );

bool prefix(const char *pre, const char *str);

bool is_keyword( char * word );

bool is_data_type( char * t_str );

void remove_spaces(char* s);

int get_data_type(char *value);
#endif