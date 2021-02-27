#include "../headers/ddl_parser.h"
#include "../headers/storagemanager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Parse through the Create Table command.
 * @param command - the command to parse.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseCreate(char *command, char **token);

/**
 * Parse through the Drop Table command.
 * @param command - the command to parse.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseDrop(char *command, char **token);

/**
 * Parse through the Alter Table command.
 * @param command - the command to parse.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseAlter(char *command, char **token);

/**
 * Parse through the SQL statement the user entered.
 * @param statement - the statement to parse.
 * @return 0 upon success, -1 upon error.
 */
int parseStatement(char *statement);

/**
 * This function handles the parsing of DDL statements
 *
 * @param statement - the DDL statement to execute
 * @return 0 on success; -1 on failure.
 */
int parse_ddl_statement(char *statement) {
    printf("%s\n", statement);

    int result = 0;
    if (strcmp(statement, "") == 0) {
        fprintf(stderr, "No command entered.\n");
        return -1;
    }

    char *statement_token;
    char *statement_copy = malloc(sizeof(char *) * strlen(statement) + 1);
    strcpy(statement_copy, statement);

    // Read in first part of statement through semicolon
    char *statement_parser = strtok_r(statement_copy, ";", &statement_token);

    // Read through all statements ending with ';'
    while (statement_parser != NULL) {
        result = parseStatement(statement_parser);
        statement_parser = strtok_r(NULL, ";", &statement_token);
    }

    free(statement_copy);
    return result;
}

int parseStatement(char *statement) {
    printf("%s\n", statement);
    int result = 0;
    char *command = malloc(sizeof(char *) * strlen(statement) + 1);
    strcpy(command, statement);

    // token to be used to handle the string tokenizing
    char *command_token;
    char *command_parser = strtok_r(command, " ", &command_token);

    // Read through each command within the statement
    while (command_parser != NULL) {

        // parse the DROP TABLE command
        if (strcasecmp(command_parser, "drop") == 0) {
            result = parseDrop(command_parser, &command_token);
        }
        // parse the ALTER TABLE command
        else if (strcasecmp(command_parser, "alter") == 0) {
            result = parseAlter(command_parser, &command_token);
        }
        // parse the CREATE TABLE command.
        else if (strcasecmp(command_parser, "create") == 0) {
            result = parseCreate(command_parser, &command_token);
        } else {
            fprintf(stderr, "Error: Invalid command.\n");
            free(command);
            return -1;
        }

        command_parser = strtok_r(NULL, " ", &command_token);
    }

    free(command);
    return result;
}

int parseDrop(char *command, char **token) {
    command = strtok_r(NULL, " ", token);
    if (strcasecmp(command, "table") == 0) {

        // read in the table name
        command = strtok_r(NULL, " ", token);

        // read table from disk
        // get table id
        // drop the table with table od
        // return drop_table(0);
        return 0;
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

int parseAlter(char *command, char **token) {
    return 0;
}

int parseCreate(char *command, char **token) {
    return 0;
}