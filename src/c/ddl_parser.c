#include "../headers/ddl_parser.h"
#include "../headers/ddl_manager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This function handles the parsing of DDL statements
 *
 * @param statement - the DDL statement to execute
 * @return 0 on success; -1 on failure.
 */
int parse_ddl_statement(char *statement) {

    if(strcasecmp(statement, "quit;") == 0){
        return 0;
    }

    int result = 0;
    if (strcmp(statement, "") == 0) {
        fprintf(stderr, "No command entered.\n");
        return -1;
    }

    char *statement_token;
    char *statement_copy = malloc(strlen(statement) + 1);
    strcpy(statement_copy, statement);

    // Read in first part of statement through semicolon
    char *statement_parser = strtok_r(statement_copy, ";", &statement_token);

    // Read through all statements ending with ';'
    while (statement_parser != NULL) {
//        printf("Parsing:\n%s\n\n", statement_parser);
        result = parseStatement(statement_parser);
        statement_parser = strtok_r(NULL, ";", &statement_token);
    }

    free(statement_copy);
    return result;
}