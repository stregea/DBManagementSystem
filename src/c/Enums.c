#include "../headers/Enums.h"
#include <string.h>

int get_type(char *type) {

    if (strcasecmp(type, "integer") == 0) {
        return INTEGER;
    } else if (strcasecmp(type, "double") == 0) {
        return DOUBLE;
    } else if (strcasecmp(type, "bool") == 0) {
        return BOOL;
    } else if (strcasecmp(type, "char") == 0) {
        return CHAR;
    } else if (strcasecmp(type, "varchar") == 0) {
        return VARCHAR;
    }
    return INVALID;
}

int get_operation(char operator) {
    if (operator == '+') {
        return ADDITION;
    } else if (operator == '-') {
        return SUBTRACTION;
    } else if (operator == '*') {
        return MULTIPLICATION;
    } else if (operator == '/') {
        return DIVISION;
    }
    return INVALID;
}

int get_conditional(char *conditional) {
    if (strcmp(conditional, "=") == 0) {
        return EQUALS;
    } else if (strcmp(conditional, ">") == 0) {
        return GREATER_THAN;
    } else if (strcmp(conditional, ">=") == 0) {
        return GREATER_THAN_OR_EQUAL_TO;
    } else if (strcmp(conditional, "<") == 0) {
        return LESS_THAN;
    } else if (strcmp(conditional, "<=") == 0) {
        return LESS_THAN_OR_EQUAL_TO;
    }
    return INVALID;
}
