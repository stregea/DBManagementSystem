#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "catalog.h"

#ifndef DBMANAGEMENTSYSTEM_DDL_MANAGER_H
#define DBMANAGEMENTSYSTEM_DDL_MANAGER_H

/**
 * Parse through the SQL statement the user entered.
 * @param statement - the statement to parse.
 * @return 0 upon success, -1 upon error.
 */
int parseStatement(char *statement);

/**
 * Parse through the Create Table command.
 * @param tokenizer - the tokenizer to parse a command.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseCreate(char *tokenizer, char **token);

/**
 * Parse through the Drop Table command.
 * @param command - the command to parse.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseDrop(char *tokenizer, char **token);

/**
 * Parse through the Alter Table command.
 * @param tokenizer - the tokenizer to parse a command.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseAlter(char *tokenizer, char **token);

/**
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of 'primarykey( <a_1> ... <a_N> )' and will generate a
 * key_indices[] for a table.
 * @param table - The table to reference.
 * @param names - The tokenizer containing the string of attribute names to be parsed.
 * @return 0 on success; -1 on error.
 */
int parsePrimaryKey(Table table, char *names);

/**
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of 'foreignkey( <a_1> ... <a_N> )' and will generate a foreign key to
 * reference another table.
 * @param table - The table to reference.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @return 0 on success; -1 on error.
 */
int parseForeignKey(Table table, char *tokenizer, char **token);

/**
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of 'foreignkey( <a_1> ... <a_N> )' and will generate a foreign key to
 * reference another table.
 * @param table - The table to reference.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @return 0 on success; -1 on error.
 */
int parseUnique(Table table, char *names);

/**
 * Given a query:
 *  create table <name>(
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *      primarykey( <a_1> ... <a_N> ),
 *      unique( <a_1> ... <a_N> ),
 *      foreignkey( <a_1> ... <a_N> ) references <r_name>( <r_1> ... <r_N> )
 *  );
 *
 * This function will handle the parsing of '<a_name> <a_type> <constraint_1> ... <constraint_N>,
 *                                           <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *                                           <a_name> <a_type> <constraint_1> ... <constraint_N>,
 *                                           <a_name> <a_type> <constraint_1> ... <constraint_N>,'
 * and properly store the values within the table.
 * @param table - The table to reference.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @return 0 on success; -1 on error.
 */
int parseAttributes(Table table, char *tokenizer);

/**
 * Determine if 'char' or 'varchar' exist in a string.
 * @param tokenizer - The tokenizer containing the string to be parsed.
 * @param token - The token used for string tokenizing.
 * @return a new string; null if not char/varchar
 */
int char_or_varchar(char*tokenizer);

int is_valid_name(char *name);
#endif
