#include "../headers/database.h"
#include "../headers/catalog.h"
#include "../headers/dml_parser.h"
#include "../headers/ddl_parser.h"
#include "../headers/arrays.h"
#include "../headers/shunting_yard_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char * expression = "21*34+1/2";
    char * string = malloc(strlen(expression) + 1);
    strcpy(string, expression);
    StringArray strings = expression_to_string_list(string);
    strings = infix_to_postfix(strings);

    return 0;
}