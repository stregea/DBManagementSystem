#ifndef DBMANAGEMENTSYSTEM_SELECT_PARSER_H
#define DBMANAGEMENTSYSTEM_SELECT_PARSER_H

//
// TODO: This file will handle the main functionality for parsing a select statement.
//      The idea is to split the key words into their own functions, and once 1 of the
//      3 keywords are read, the corresponding functions will be called.
//

/**
 * Parse the 'select' keyword.
 *
 * This will handle '*' or n-amount of specified columns.
 * @param statement - The statement to parse.
 * @return 0 on success, -1 on error.
 */
int parse_select(char *statement);

/**
 * Parse the 'from' keyword.
 *
 * This will handle the parsing of the n-amount of tables
 * specified within a statement.
 * @param statement - The statement to parse.
 * @return 0 on success, -1 on error.
 */
int parse_from(char *statement);

/**
 * Parse the 'where' clause.
 *
 * This will handle the where clause, so this will be able to implement
 * sub-queries as well as different boolean expressions.
 * @param statement - The statement to parse.
 * @return 0 on success, -1 on error.
 */
int parse_where(char *statement);

#endif
