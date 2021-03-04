#include "../headers/ddl_parser.h"
#include "../headers/storagemanager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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


/**
 * Parse through the SQL statement the user entered.
 * @param statement - the statement to parse.
 * @return 0 upon success, -1 upon error.
 */
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

/**
 * Parse through the Drop Table command.
 * @param command - the command to parse.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseDrop(char *command, char **token) {
    command = strtok_r(NULL, " ", token);
    // table <name>
    if (strcasecmp(command, "table") == 0) {

        // read in the table name
        command = strtok_r(NULL, " ", token);

        if (command != NULL && strcasecmp(command, "") != 0) {
            // read table from disk
            // get table id
            // drop the table with table od
            // drop_table(0);
            // TODO
        } else {
            // no table specified name
            return -1;
        }

        return 0;
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

/**
 * Parse through the Alter Table command.
 * // TODO: Update the code. It looks hideous.
 * @param tokenizer - the tokenizer to parse a command.
 * @param token - The token used for string tokenizing.
 * @return 0 upon success, -1 upon error.
 */
int parseAlter(char *tokenizer, char **token) {
    tokenizer = strtok_r(NULL, " ", token);
    if (strcasecmp(tokenizer, "table") == 0) {

        // read in the table name
        tokenizer = strtok_r(NULL, " ", token);

        if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
            // read in function type
            tokenizer = strtok_r(NULL, " ", token);

            //drop <name>
            if (strcasecmp(tokenizer, "drop") == 0) {
                // read attribute name
                tokenizer = strtok_r(NULL, " ", token);

                if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
                    // TODO
                    // read table from disk
                }
            }

            // add <a_name> <a_type>
            // add <a_name> <a_type> default <value>
            else if (strcasecmp(tokenizer, "add") == 0) {
                // read <a_name>
                tokenizer = strtok_r(NULL, " ", token);
                if (tokenizer != NULL) {
                    char *a_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    char *a_type;
                    char *_default;
                    char *value;

                    strcpy(a_name, tokenizer);

                    // read <a_type>
                    tokenizer = strtok_r(NULL, " ", token);
                    if (tokenizer != NULL) {
                        a_type = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                        strcpy(a_type, tokenizer);

                        // read default (this is optional)
                        tokenizer = strtok_r(NULL, " ", token);
                        if (tokenizer != NULL) {
                            _default = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                            strcpy(_default, tokenizer);

                            // read <value>
                            tokenizer = strtok_r(NULL, " ", token);
                            if(tokenizer != NULL){
                                value = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                                strcpy(value, tokenizer);

                                /**
                                 * TODO:
                                 *  will add an attribute
                                 *  with the given name and data type to the table; as long as an attribute with that name
                                 *  does not exist already. It will then will add the default value for that attribute to all
                                 *  existing tuples in the database. The data type of the value must match that of the
                                 *  attribute, or its an error
                                 */
                                free(value);
                                free(_default);
                                free(a_type);
                                free(a_name);
                                return 0; // proper command structure.
                            }

                            free(_default); // error since no default value specified.
                        }

                        /**
                         * TODO:
                         *  will add an attribute with the given name
                         *  and data type to the table; as long as an attribute with that name does not exist
                         *  already. It will then will add a null value for that attribute to all existing tuples in the
                         *  database
                         */
                        free(a_type); // since no type specified.
                        free(a_name);
                        return 0; // proper structure.
                    }
                    free(a_name); // error since no name specified.
                }
                return -1; // error invalid command. No type specified.
            } else {
                // invalid command
                fprintf(stderr, "Error: Invalid command.\n");
                return -1;
            }

        } else {
            // no table specified name
            fprintf(stderr, "Error: No table specified.\n");
            return -1;
        }
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

int parseCreate(char *command, char **token) {
    // TODO
    return 0;
}