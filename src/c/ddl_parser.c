#include "../headers/ddl_parser.h"
#include "../headers/storagemanager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Struct to contain foreign key information.
 */
struct ForeignKey {
    char *table_name; // name of referenced table
    char *column_name; // name of referenced column
};

struct Constraints {
    bool notnull;
    bool primary_key;
    bool unique;
};

/**
 * Struct to contain information in regards to a attribute/column.
 */
struct Attribute {
    char *name; // name of the attribute (column)
    int type; // the type of data within the column (0-4) / int-varchar
    int size; // used to determine the size of a char or varchar.
    struct Constraints constraints;
};

/**
 * Struct used to represent a Table.
 *
 * This is created within the create table command, or read from the catalog.
 */
struct Table {
    int tableId;
    char *name; // the name of the table
    struct Attribute *attributes; // Array to hold the attributes/columns of a table.
    int *key_indices; // array to contain the information to create a primary key.
    int *data_types; // array to contain the data types found within a table.
    union record_item **unique; // 2-D array to contain only the unique tuples that can be found in the table.
    struct ForeignKey *foreignKey; // foreign key to reference another table.
    int primary_key_count; // count used to kep track of the # of primary keys that exist within a table. 1 Max.
    int foreign_key_count; // count used to kep track of the # of foreign keys that exist within a table.
    int attribute_count; // count used to kep track of the # of atributes/columns that exist within a table.
};
// TODO: Catalog.

void freeTable(struct Table table);

int get_attribute_type(char *attribute);

int add_primary_key_to_table(struct Table table, int *key_indices);

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

int parseAttributes(struct Table table, char *tokenizer);

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
//    printf("%s\n", statement);
    int result = 0;
    char *command = malloc(strlen(statement) + 1);
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
int parseDrop(char *tokenizer, char **token) {
    tokenizer = strtok_r(NULL, " ", token);
    // table <name>
    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {
        printf("%s\n", tokenizer);

        // read in the table name
        tokenizer = strtok_r(NULL, " ", token);

        if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
            printf("%s\n", tokenizer);
            // TODO
            // read table from catalog
            // get table id
            // drop the table with table id
            // drop_table(0);
            char *table_name = malloc(strlen(tokenizer) + 1);
            strcpy(table_name, tokenizer);
            // read table from disk

            free(table_name);
            return 0;
        }
        // no table specified name
        return -1;
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

    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {
        printf("%s\n", tokenizer);

        // read in the table name
        tokenizer = strtok_r(NULL, " ", token);

        if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
            printf("%s\n", tokenizer);
            // read in function type
            tokenizer = strtok_r(NULL, " ", token);

            //drop <name>
            if (tokenizer != NULL && strcasecmp(tokenizer, "drop") == 0) {
                printf("%s\n", tokenizer);

                // read attribute name
                tokenizer = strtok_r(NULL, " ", token);

                if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
                    printf("%s\n", tokenizer);

                    // TODO
                    char *table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    strcpy(table_name, tokenizer);
                    // read table from disk

                    free(table_name);
                    return 0; // correct structure
                }
                return -1; // incorrect syntax/structure
            }

                // add <a_name> <a_type>
                // add <a_name> <a_type> default <value>
            else if (tokenizer != NULL && strcasecmp(tokenizer, "add") == 0) {
                printf("%s\n", tokenizer);
                // read <a_name>
                tokenizer = strtok_r(NULL, " ", token);
                if (tokenizer != NULL) {
                    printf("%s\n", tokenizer);
                    char *a_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    char *a_type;
                    char *_default;
                    char *value;

                    strcpy(a_name, tokenizer);

                    // read <a_type>
                    tokenizer = strtok_r(NULL, " ", token);

                    if (tokenizer != NULL) {
                        printf("%s\n", tokenizer);
                        a_type = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                        strcpy(a_type, tokenizer);

                        // read default (this is optional)
                        tokenizer = strtok_r(NULL, " ", token);

                        if (tokenizer != NULL && strcasecmp(tokenizer, "default") == 0) {
                            printf("%s\n", tokenizer);
                            _default = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                            strcpy(_default, tokenizer);

                            // read <value>
                            tokenizer = strtok_r(NULL, " ", token);

                            if (tokenizer != NULL) {
                                printf("%s\n", tokenizer);
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

int parseCreate(char *tokenizer, char **token) {
    // TODO

    struct Table table_data; // add create_table function?
    table_data.attribute_count = 0;
    table_data.primary_key_count = 0;
    table_data.foreign_key_count = 0;
    table_data.attributes = malloc(sizeof(struct Attribute));

    tokenizer = strtok_r(NULL, " ", token);

    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {

        // read in Table name
        tokenizer = strtok_r(NULL, " (", token);
        if (tokenizer != NULL) {
            table_data.name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
            strcpy(table_data.name, tokenizer);

            // perform loop to read in rest of attributes and populate Table struct
            while (tokenizer != NULL) {

                /**
                 *
                 * CREATE TABLE BAZZLE( baz double PRIMARYKEY ); // need to optimize parser for this later
                 *
                 *   create table foo( // working on parsing this
                 *       baz integer,
                 *       bar Double notnull,
                 *       primarykey( bar baz ),
                 *       foreignkey( bar ) references bazzle( baz )
                 *   );
                 *
                 *
                 */
                // read in table information
                tokenizer = strtok_r(NULL, "(,", token);

                // this can potentially be null -> prevent any potential segfaults for bad reads
                if (tokenizer != NULL) {

                    // check for any keywords
                    if (strcasecmp(tokenizer, "primarykey") == 0) {
                        // set and create table primary key
                        // may want to create a function here
                        tokenizer = strtok_r(NULL, "(,", token);
                    } else if (strcasecmp(tokenizer, "foreignkey") == 0) {
                        // set delim as ), in tokenizer
                        // set and create foreign key
                        tokenizer = strtok_r(NULL, "(,", token);
                        // parse references table_name(table_col)
                        // may want to create a function here
                    } else if (strcasecmp(tokenizer, "unique") == 0) {
                        // set delim as ), in tokenizer
                        // set unique attributes
                        // may want to create a function here
                    } else { // parse through the attributes/column information
                        int result = parseAttributes(table_data, tokenizer);
                        if (result == -1) {
                            freeTable(table_data);
                            return -1;
                        }
                    }
                }
            }
            // write table to catalog

            freeTable(table_data);
            return 0;
        }
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

// TODO
int parseAttributes(struct Table table, char *tokenizer) {
    printf("%s\n", tokenizer);

    char* temp_token;
    tokenizer = strtok_r(tokenizer, " ", &temp_token); // split the string via spaces
    struct Attribute attribute;

    // read in attribute/ column name
    if (tokenizer != NULL) {
        attribute.name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
        attribute.constraints.primary_key = false;
        attribute.constraints.notnull = false;
        attribute.constraints.unique = false;

        strcpy(attribute.name, tokenizer);

        // read in attribute type
        tokenizer = strtok_r(NULL, " ", &temp_token);
        if (tokenizer != NULL) {

            // read in column type, function returns 0-4 based on string name (integer-varchar)
            attribute.type = get_attribute_type(tokenizer);

            // if type == -1 -> error
            // if type == char || varchar
            // TODO
            if (attribute.type == 3 || attribute.type == 4) {
                // set size of attribute
            }

            // loop through constraints
            while (tokenizer != NULL) {
                tokenizer = strtok_r(NULL, " )", &temp_token);

                if (tokenizer != NULL) {
                    // TODO
                    if (strcasecmp(tokenizer, "primarykey") == 0) {
                        attribute.constraints.primary_key = true;
                        // TODO: set primary key for table
                    } else if (strcasecmp(tokenizer, "unique") == 0) {
                        // TODO: add unique contraint to table
                        attribute.constraints.unique = true;
                    } else if (strcasecmp(tokenizer, "notnull") == 0) {
                        // flag attribute as notnull
                        attribute.constraints.notnull = true;
                    }
                    // else error?
                }
            }
        }
        table.attribute_count++;
        table.attributes = realloc(table.attributes,
                                   sizeof(struct Attribute)*table.attribute_count); // this is currently not being set anywhere. need to set default to 0
        table.attributes[table.attribute_count-1] = attribute;
        // read as column/attribute // set delim as ), in tokenizer
        // split string through space
        // read in column name
        return 0;
    }
    return -1;
}

void freeTable(struct Table table) {
    free(table.name);
    free(table.key_indices);
    free(table.data_types);
    // TODO: free unique array -- not to sure how I want to save this yet.

    // free foreign keys
    for (int i = 0; i < table.foreign_key_count; i++) {
        free(table.foreignKey[i].table_name);
        free(table.foreignKey[i].column_name);
    }

    // free attributes
    for (int i = 0; i < table.attribute_count; i++) {
        free(table.attributes[i].name);
//        free(table.attributes[i].constraints);
    }
}

int add_primary_key_to_table(struct Table table, int *key_indices) {

    if (table.primary_key_count == 0) {
        table.key_indices = key_indices;
    }
    return -1; // error since primary key already exists in table.
}

int get_attribute_type(char *type) {

    if (strcasecmp(type, "integer") == 0) {
        return 0;
    } else if (strcasecmp(type, "double") == 0) {
        return 1;

    } else if (strcasecmp(type, "bool") == 0) {
        return 2;

    } else if (strcasecmp(type, "bool") == 0) {
        return 3;

    } else if (strcasecmp(type, "bool") == 0) {
        return 4;
    }
    return -1;
}