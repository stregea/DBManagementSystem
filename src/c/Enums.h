#ifndef DBMANAGEMENTSYSTEM_ENUMS_H
#define DBMANAGEMENTSYSTEM_ENUMS_H

// All of the valid types tp be used.
enum Types{
    INTEGER = 0, DOUBLE = 1, BOOL = 2, CHAR = 3, VARCHAR = 4
};

// Mathematics
enum Math {
    ADDITION = 0, SUBTRACTION = 1, MULTIPLICATION = 2, DIVISION = 3
};

// Conditionals
enum Conditionals {
    EQUALS = 0, GREATER_THAN = 1, GREATER_THAN_OR_EQUAL_TO = 2, LESS_THAN = 3, LESS_THAN_OR_EQUAL_TO = 4
};
#endif
