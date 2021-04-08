#include "../headers/database.h"
#include "../headers/catalog.h"
#include "../headers/dml_parser.h"
#include "../headers/ddl_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TOKEN_SIZE 20

/*
 * This function will be used to execute SQL statements that
 * do not return any data. For instance, schema modification,
 * insertion, delete, and update statements.
 *
 * @param statement - the SQL statement to execute.
 * @return 0 if the statement is executed successfully
           -1 otherwise
 */
int execute_non_query(char *statement) {
    char *tmp_statement = malloc(strlen(statement) + 1);
    strcpy(tmp_statement, statement);

    char *first_word = strtok(tmp_statement, " ");

    if (strcasecmp(first_word, "create") == 0 || strcasecmp(first_word, "alter") == 0 ||
        strcasecmp(first_word, "drop") == 0) {
        free(tmp_statement);
        return parse_ddl_statement(statement);
    }
    free(tmp_statement);
    return parse_dml_statement(statement);
}

/**
 * TODO
 * This function will be used to execute SQL statement that
 * return data. For instance, SQL select queries.
 *
 * @param query - the SQL query to execute
 * @param result - a 2d array of record_item (output variable).
 *                This will be used to output the values of the query.
                  This will be a pointer to the first item in the 2d array.
                  The user of the function will be resposible for freeing.
 * @return the number of tuples in the result, -1 if upon error.
 */
int execute_query(char *query, union record_item ***result) {
    return 0;
}

/**
 * Parse a statement and execute the query based on the first word of the command.
 * @param statement - The statement to parse.
 * @return 0 on success, -1 on error.
 */
int parse_statement(char *statement) {

    if (strcasecmp(statement, "quit;") == 0){
        return 0;
    }

    char *tmp_statement = malloc(strlen(statement) + 1);
    strcpy(tmp_statement, statement);

    char *first_word = strtok(tmp_statement, " ");

    if (strcasecmp(first_word, "select") == 0) {
        free(tmp_statement);
        return execute_query(statement, NULL); // TODO
    }

    free(tmp_statement);
    return execute_non_query(statement);
}

/*
 * This function is responsible for safely shutdown the database.
 * It will store to hardware any data needed to restart the database.
 * @return 0 on success; -1 on failure.
 */
int shutdown_database() {
    int result = 0;
    if (shutdown_catalog() == -1) {
        fprintf(stderr, "%s", "Error shutting down the catalog.\n");
        result = -1;
    }
    if (terminate_database() == -1) {
        fprintf(stderr, "%s", "Error shutting down the storage manager.\n");
        result = -1;
    }
    return result;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "%s", "usage: ./database <db_loc> <page_size> <buffer_size>\n");
        return -1;
    }

    char *databasePath = argv[1];
    int pageSize = (int) strtol(argv[2], NULL, 10);
    int bufferSize = (int) strtol(argv[3], NULL, 10);

    printf("db_loc: %s\npage_size: %d\nbuffer_size: %d\n", databasePath, pageSize, bufferSize);

    char *catalog_file_name = "/catalog";
    char *catalog_path;

    // format the catalog file name and path
    int database_path_length = strlen(databasePath);
    char last_character = databasePath[database_path_length - 1];
    if (last_character == '\'' || last_character == '/') {
        databasePath[database_path_length - 1] = 0;
    }

    catalog_path = malloc(strlen(databasePath) + strlen(catalog_file_name) + 1);

    strcpy(catalog_path, databasePath);
    strcat(catalog_path, catalog_file_name);

    bool restart = false;

    if (access(catalog_path, F_OK) == 0) {
        restart = true;
    }

    initialize_catalog(databasePath, restart);
    create_database(databasePath, pageSize, bufferSize, restart);
    free(catalog_path);

    // initialize with first token
    char token[MAX_TOKEN_SIZE];
    scanf("%s", token);
    int tokenLength = strlen(token);

    int statementCount = 0;

    while (strcmp(token, "quit;") != 0) {

        // read in next token for next statement
        if (statementCount > 0) {
            scanf("%s", token);
            //printf("%s", token);
            tokenLength = (int) strlen(token);
        }

        // initialize statement string
        int statementLength = tokenLength;
        char *statement = malloc(statementLength + 1);
        strncpy(statement, token, statementLength + 1);

        // temp to hold statement contents when reading in statement
        char *temp = malloc(statementLength + 1);

        // stop if the ';' character is within the token
        while (token[tokenLength - 1] != ';') {

            // read next token
            scanf("%s", token);
            //printf("%s", token);
            tokenLength = strlen(token);

            // copy statement string to temp string
            temp = realloc(temp, statementLength + 1);
            strncpy(temp, statement, statementLength + 1);

            // resize the statement memory to hold the contains in new token
            // memblock = [Statement] + [sizeofchar] + [Token] + [NullTerminator]
            statement = realloc(statement, statementLength + sizeof(char) + tokenLength + 1);
            strncpy(statement, temp, statementLength + 1);
            statementLength = statementLength + sizeof(char) + tokenLength;
            strncat(statement, " ", statementLength + 1);
            strncat(statement, token, statementLength + 1);

        }

        printf("\nStatement:\n%s\n\n", statement);

        int result = parse_statement(statement);

        if (result == -1) {
            fprintf(stderr, "ERROR\n");
        } else {
            fprintf(stdout, "SUCCESS\n");
        }

        free(statement);
        free(temp);
        statementCount++;
    }

    shutdown_database();
}