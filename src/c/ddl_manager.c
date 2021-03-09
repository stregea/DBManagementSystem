//
// Created by sdtre on 3/8/2021.
//

#include "../headers/ddl_manager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//static int num_tables;
static char *global_db_loc;

// TODO: allocate memory on startup and deallocate on shutdown
//static struct Table **catalog = NULL;
static Catalog catalog = NULL;

// todo
int initialize_ddl_parser(char *db_loc, bool restart) {
    global_db_loc = malloc(strlen(db_loc) + 1);
    strcpy(global_db_loc, db_loc);

//    if(restart){
//        return read_catalog_from_disk(); // this has yet to be defined
//    }

    // otherwise a new database

    // set table count to 0.
    //num_tables = 0;

    // init catalog to have 1 slot of memory
    // whenever a table is added, realloc must be called.
    //catalog = malloc(sizeof(Table) * 1);
    //catalog[0] = NULL;

    return 0;
}

// todo
int terminate_ddl_parser() {
    int result = 0;
//    result = write_catalog_to_disk(); // not yet defined
//    freeCatalog(); // not yet defined
    return result;
}

/**
 * Parse through the SQL statement the user entered.
 * @param statement - the statement to parse.
 * @return 0 upon success, -1 upon error.
 */
int parseStatement(char *statement) {
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
            fprintf(stderr, "Error: Invalid command.\n"); // remove this later
            free(command);
            return -1;
        }

        command_parser = strtok_r(NULL, " ", &command_token);
    }
    free(command);
    return result;
}

/**
 * TODO
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
//            Table table = get_table_from_catalog(tokenizer);
//            drop_table(table->tableId);
//            freeTable(table);
//
            // TODO
            // update the catalog to remove reference of table
            return 0;
        }
        // no table specified name
        return -1;
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

/**
 * TODO
 * Parse through the Alter Table command.
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

                    char *table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    strcpy(table_name, tokenizer);

                    // TODO
                    // read table from catalog
                    // update the table
                    // save the table to catalog

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
                                 * TODO: Add attribute to table
                                 * <name> add <a name> <a type> default <value>
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
                         * TODO: Add attribute to table
                         * <name> add <a name> <a type>
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

// TODO
int parseCreate(char *tokenizer, char **token) {

    tokenizer = strtok_r(NULL, " ", token);

    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {

        // read in Table name
        tokenizer = strtok_r(NULL, " (", token);
        if (tokenizer != NULL) {

            // adding table to catalog will create its ID
            struct Table *table_data = createTable(tokenizer);

            // perform loop to read in rest of attributes and populate Table struct
            while (tokenizer != NULL) {

                /**
                 * TODO: This is for referencing an example query for parsing
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
                tokenizer = strtok_r(NULL, "(),", token);

                // this can potentially be null -> prevent any potential segfaults for bad reads
                if (tokenizer != NULL) {

                    // check for any keywords
                    if (strcasecmp(tokenizer, "primarykey") == 0) {
                        // TODO: test
                        // read in attribute names
                        tokenizer = strtok_r(NULL, "(),", token);
                        parsePrimaryKey(table_data, tokenizer);
                    } else if (strcasecmp(tokenizer, "foreignkey") == 0) {
                        // TODO parse foreignkey(...)
                        // read in attribute names
                        tokenizer = strtok_r(NULL, "(),", token);
                        // then parse references table_name(table_col) -- do this parsing within parseForeignKey
                        // may want to create a function here
                        // parseForeignKey(table_data, tokenizer);
                    } else if (strcasecmp(tokenizer, "unique") == 0) {
                        // TODO parse unique(...)
                        // read in attribute names
                        tokenizer = strtok_r(NULL, "(),", token);
                        // set unique attributes
//                        parseUnique(table_data, tokenizer);
                    } else { // parse through the attributes/column information
                        int result = parseAttributes(table_data, tokenizer);
                        if (result == -1) {
                            freeTable(table_data);
                            return -1;
                        }
                    }
                }
            }
            // TODO: write table to catalog
            freeTable(table_data);
            return 0;
        }
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

// todo: finalize testing -- need to make sure created primary key is correct.
int parsePrimaryKey(Table table, char *names) {
    // set and create table primary key
    return add_primary_key_to_table(table, create_primary_key(names, table));
}

// TODO
int parseAttributes(Table table, char *tokenizer) {
    printf("%s\n", tokenizer);

    char *temp_token;
    tokenizer = strtok_r(tokenizer, " ", &temp_token); // split the string via spaces
    struct Attribute attribute;

    // read in attribute/ column name
    if (tokenizer != NULL) {
        attribute.name = malloc(
                sizeof(char *) * strlen(tokenizer) + 1);
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
                // read in tokenizer, parse size of char/varchar
                // set size of attribute
            }

            // loop through constraints
            while (tokenizer != NULL) {
                tokenizer = strtok_r(NULL, " )", &temp_token);

                if (tokenizer != NULL) {
                    // TODO
                    if (strcasecmp(tokenizer, "primarykey") == 0) {
                        attribute.constraints.primary_key = true;
                        // TODO: test
//                        add_primary_key_to_table(table, create_primary_key(attribute.name, table));
                    } else if (strcasecmp(tokenizer, "unique") == 0) {
                        // TODO: add unique contraint to table
                        attribute.constraints.unique = true;
                    } else if (strcasecmp(tokenizer, "notnull") == 0) {
                        // flag attribute as notnull
                        attribute.constraints.notnull = true;
                    } else {
                        // else error since invalid token?
                        free(attribute.name);
                        return -1;
                    }
                }
            }
        }
        table->attribute_count++;
        table->attributes = realloc(table->attributes,
                                    sizeof(struct Attribute) * table->attribute_count + 1);
        table->attributes[table->attribute_count - 1] = attribute;
        return 0;
    }
    return -1;
}

void freeTable(Table table) {
    free(table->name);
    free(table->data_types);
    free(table->key_indices);
    // TODO: free unique array -- not to sure how I want to save this yet.

    // free foreign keys
    for (int i = 0; i < table->foreign_key_count; i++) {
        free(table->foreignKey[i].table_name);
        free(table->foreignKey[i].column_name);
    }
    free(table->foreignKey);

    // free attributes
    for (int i = 0; i < table->attribute_count; i++) {
        free(table->attributes[i].name);
//        free(table.attributes[i].constraints);
    }
    free(table->attributes);
    free(table);
}

int get_attribute_type(char *type) {

    if (strcasecmp(type, "integer") == 0) {
        return 0;
    } else if (strcasecmp(type, "double") == 0) {
        return 1;

    } else if (strcasecmp(type, "bool") == 0) {
        return 2;

    } else if (strcasecmp(type, "char") == 0) {
        return 3;

    } else if (strcasecmp(type, "varchar") == 0) {
        return 4;
    }
    return -1;
}

// TODO: this hasn't been tested.
PrimaryKey create_primary_key(char *attribute_names, Table table) {
    PrimaryKey key = malloc(sizeof(struct PrimaryKey));
    key->key_indices = NULL;
    int index_count = 0;
    char *tokenizer = strtok(attribute_names, " "); // split the attributes on space
    while (tokenizer != NULL) {
        for (int i = 0; i < table->attribute_count; i++) {
            if (strcasecmp(tokenizer, table->attributes[i].name) == 0) {
                key->key_indices = realloc(key->key_indices, sizeof(int *) * (index_count + 1));
                key->key_indices[index_count++] = table->attributes[i].type;
            }
        }
        tokenizer = strtok(NULL, " ");
    }
    key->key_indices_count = index_count;
    return key;
}

// todo: test
int add_primary_key_to_table(Table table, PrimaryKey key) {
    if (key->key_indices != NULL && table->primary_key_count == 0) {
        table->key_indices = realloc(table->key_indices, key->key_indices_count * sizeof(int *));
        memcpy(table->key_indices, key->key_indices, key->key_indices_count * sizeof(int *));
        table->primary_key_count++;
        table->key_indices_count = key->key_indices_count;
        free(key->key_indices);
        free(key);
        return 0;
    }
    free(key);
    return -1; // error since primary key already exists in table.
}

void createCatalog(Table table) {
    
    catalog = malloc(sizeof(struct Catalog));

    // allocate memory for struct attributes
    catalog->table_count = 1;
    catalog->tables = malloc(sizeof(Table));

    // add initial table
    catalog->tables[0] = table;
}

Table get_table_from_catalog(char *table_name) {
    for (int i = 0; i < catalog->table_count; i++) {
        if (strcmp(catalog->tables[i]->name, table_name) == 0) {
            return catalog->tables[i];
        }
    }
    return NULL;
}

int add_table_to_catalog(Table table) {
    realloc(catalog->tables, sizeof(Table) * (catalog->table_count + 1));
    catalog->tables[catalog->table_count] = table;
    catalog->table_count++;
    return 0;
}

int remove_table_from_catalog(char *table_name) {

    int loc = -1;
    for (int i = 0; i < catalog->table_count; i++) {
        if (strcmp(catalog->tables[i]->name, table_name) == 0) {
            // Nullify position in list of tables
            catalog->tables[i] = NULL;
            // Keep track of where the position was
            loc = i;
        }
    }

    // If table was found in list
    if (loc >= 0) {
        for (int i = loc; i < catalog->table_count - 1; i++) {
            // Move everything over one to fill in space
            catalog->tables[i] = catalog->tables[i+1];
        }

        // Success
        return 0;
    } else {
        // Table was not found in list
        return -1;
    }
}

struct Table * createTable(char *name){
    struct Table * table_data = malloc(sizeof(struct Table));

    // set counts and allocate memory
    table_data->attribute_count = 0;
    table_data->primary_key_count = 0;
    table_data->foreign_key_count = 0;
    table_data->key_indices_count = 0;
    table_data->attributes = malloc(sizeof(struct Attribute));
    table_data->key_indices = malloc(sizeof(int));
    table_data->data_types = malloc(sizeof(int));
    table_data->foreignKey = malloc(sizeof(struct ForeignKey));
    table_data->name = malloc(strlen(name) + 1);
    strcpy(table_data->name, name);

    return table_data;
}