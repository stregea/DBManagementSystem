#ifndef DBMANAGEMENTSYSTEM_ENUMS_H
#define DBMANAGEMENTSYSTEM_ENUMS_H

enum Validity{
    INVALID = -1, VALID = 0
};

// All of the valid types tp be used.
enum Types{
    INTEGER = 0, DOUBLE = 1, BOOL = 2, CHAR = 3, VARCHAR = 4
};

// Math operations
enum Math {
    ADDITION = 0, SUBTRACTION = 1, MULTIPLICATION = 2, DIVISION = 3
};

// Conditional operations
enum Conditionals {
    EQUALS = 0, GREATER_THAN = 1, GREATER_THAN_OR_EQUAL_TO = 2, LESS_THAN = 3, LESS_THAN_OR_EQUAL_TO = 4, NOT_EQUALS = 5
};

/**
 * Return an integer associated with an attribute type.
 *      0 - Integer
 *      1 - Double
 *      2 - Bool
 *      3 - Char
 *      4 - Varchar
 * @param type - The attribute type to parse.
 * @return a number between 0-4; -1 if an invalid type.
 */
int get_type(char *type);

/**
 * Return an integer associated with a mathematical operation.
 *      0 - Addition
 *      1 - Subtraction
 *      2 - Multiplication
 *      3 - Division
 * @param operator - The operator to parse.
 * @return a number between 0-3; -1 if an invalid operation.
 */
int get_operation(char operator);

/**
 * Return an integer associated with a conditional operation.
 *      0 - Equals ( = )
 *      1 - Greater than ( > )
 *      2 - Greater than or equal to ( >= )
 *      3 - Less than ( < )
 *      4 - Less than or equal to ( <= )
 * @param conditional - The conditional operation to parse.
 * @return a number between 0-4; -1 if an invalid conditional.
 */
int get_conditional(char *conditional);
#endif
