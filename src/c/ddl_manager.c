//
// Created by sdtre on 3/8/2021.
//

#include "../headers/ddl_manager.h"
#include "../headers/storagemanager.h"

//static int num_tables;
static char *global_db_loc;

// TODO: allocate memory on startup and deallocate on shutdown
//static struct Table **catalog = NULL;
static Catalog catalog = NULL;

// todo
int initialize_ddl_parser(char *db_loc, bool restart) {
    global_db_loc = malloc(strlen(db_loc) + 1);
    strcpy(global_db_loc, db_loc);

    printf("Starting DDL parser: %s\n", global_db_loc);

//    if(restart){
//        return read_catalog_from_disk(); // this has yet to be defined
//    }

    // otherwise a new database
//    catalog = malloc(sizeof(struct Catalog));
//
//    // set table count to 0.
//    catalog->table_count = 0;
//
//    // init catalog to have 1 slot of memory
//    // whenever a table is added, realloc must be called.
//    catalog->tables = malloc(sizeof(Table) * 1); // initialize array of struct Table*
//    catalog->tables[0] = NULL;
    return 0;
}

// todo
int terminate_ddl_parser() {
    int result = 0;
//    result = write_catalog_to_disk(); // not yet defined
    freeCatalog(); // not yet defined
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
 * TODO - Test
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

            // read table from catalog
            Table table = get_table_from_catalog(tokenizer);

            // drop the table in storagemanager.
//            if (table != NULL && drop_table(table->tableId) == 0) {
            // TODO - implement with storage manager.
            if (table != NULL) {
                // update the catalog to remove reference of table
                remove_table_from_catalog(table->name);
                freeTable(table);
                return 0;
            }
            return -1;
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
        //char *table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
        char *table_name;
        //printf("length right now: %zu\n", strlen(tokenizer));

        if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
            printf("%s\n", tokenizer);

            // Set table name to be used later
            // TODO free this at every return point
            table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
            strcpy(table_name, tokenizer);
            // read in function type
            tokenizer = strtok_r(NULL, " ", token);

            //drop <name>
            if (tokenizer != NULL && strcasecmp(tokenizer, "drop") == 0) {
                printf("alter operation: %s\n", tokenizer);

                // read attribute name
                tokenizer = strtok_r(NULL, " ", token);

                if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
                    printf("attribute name: %s\n", tokenizer);

                    //char *table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    //strcpy(table_name, tokenizer);

                    // TODO
                    // read table from catalog
                    Table table_to_alter = get_table_from_catalog(table_name);

                    // Error if table is not in the catalog
                    if (table_to_alter == NULL) {
                        fprintf(stderr, "Error: Given table does not exist\n");
                        free(table_name);
                        return -1;
                    }

                    for (int i = 0; i < table_to_alter->attribute_count; i++) {
                        if (strcasecmp(table_to_alter->attributes[i]->name, tokenizer) == 0) {
                            // Found correct attribute to drop

                            // Check if attribute is part of primary key
                            for (int j = 0; j < table_to_alter->primary_key_count; j++) {
                                if (table_to_alter->primary_key->attributes[j] == table_to_alter->attributes[i]) {
                                    fprintf(stderr, "Error: cannot drop attribute in primary key\n");
                                    free(table_name);
                                    return -1;
                                }
                            }

                            // Search for references to the attribute in foreign keys of all other tables
                            for (int j = 0; j < catalog->table_count; j++) {
                                // If the table has foreign keys
                            }
                            // Remove foreign key constraint if present

                            // Remove the attribute from the list of unique attributes, if present
                            // Remove the attribute from the list of attributes

                            // ===== storagemanager.c stuff =======
                        }
                    }
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
                 *       foreignkey( bar baz ) references bazzle( far faz )
                 *   );
                 *
                 *
                 */
                // read in table information
                tokenizer = strtok_r(NULL, "(),", token);

                // this can potentially be null or empty space -> prevent any potential segfaults for bad reads
                if (tokenizer != NULL && strcmp(tokenizer, " ") != 0) {

                    // Cut off any leading spaces
                    char * filtered = malloc(sizeof(char) * strlen(tokenizer));
                    strcpy(filtered, tokenizer);
                    while (filtered[0] == ' ') {
                        filtered++;
                    }

                    // check for any keywords
                    if (strcasecmp(filtered, "primarykey") == 0) {
                        // TODO: test
                        // read in attribute names
                        tokenizer = strtok_r(NULL, "(),", token);
                        parsePrimaryKey(table_data, tokenizer);
                    } else if (strcasecmp(filtered, "foreignkey") == 0) {
                        // TODO: test
                        // read in attribute names
                        tokenizer = strtok_r(NULL, "(),", token);
                        parseForeignKey(table_data, tokenizer, token);
                    } else if (strcasecmp(filtered, "unique") == 0) {
                        // TODO: test
                        // read in attribute names
                        tokenizer = strtok_r(NULL, "(),", token);
                        // set unique attributes
                        parseUniqueKey(table_data, tokenizer);
                    } else { // parse through the attributes/column information
                        int result = parseAttributes(table_data, tokenizer);
                        if (result == -1) {
                            freeTable(table_data);
                            return -1;
                        }
                    }
                }
            }
            // TODO: write table to storage manager.
//            table_data->tableId = add_table(table_data->data_types, table_data->primary_key->key_indices,
//                                            table_data->data_type_size,
//                                            table_data->primary_key->size);
            add_table_to_catalog(table_data); // this doesn't work for whatever reason.
            return 0;
        }
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

// todo: finalize testing -- need to make sure created primary key is correct.
int parsePrimaryKey(Table table, char *names) {
    printf("parsing primary key: %s\n", names);
    // set and create table primary key
    return add_primary_key_to_table(table, create_key(names, table));
}

//TODO: test
int parseUniqueKey(Table table, char *names) {
    return add_unique_key_to_table(table, create_key(names, table));
}

// TODO
int parseAttributes(Table table, char *tokenizer) {
    printf("attributes: %s\n", tokenizer);

    char *temp_token;
    tokenizer = strtok_r(tokenizer, " ", &temp_token); // split the string via spaces
    Attribute attribute = malloc(
            sizeof(struct Attribute)); // this may need to be a pointer since being saved onto the stack rather than heap

    // read in attribute/ column name
    if (tokenizer != NULL) {
        attribute->name = malloc(strlen(tokenizer) + 1);
        attribute->foreignKey = NULL;
        attribute->constraints = malloc(sizeof(struct Constraints));
        attribute->constraints->primary_key = false;
        attribute->constraints->notnull = false;
        attribute->constraints->unique = false;
        attribute->type = 0;
        strcpy(attribute->name, tokenizer);

        // read in attribute type
        tokenizer = strtok_r(NULL, " ", &temp_token);
        if (tokenizer != NULL) {

            // read in column type, function returns 0-4 based on string name (integer-varchar)
            attribute->type = get_attribute_type(tokenizer);
            // if type == -1 -> error
            // if type == char || varchar
            // TODO - set char/varchar attributes
            if (attribute->type == 3 || attribute->type == 4) {
                // read in tokenizer, parse size of char/varchar
                // set size of attribute
            }

            // loop through constraints
            while (tokenizer != NULL) {
                tokenizer = strtok_r(NULL, " )", &temp_token);

                if (tokenizer != NULL) {
                    // TODO
                    if (strcasecmp(tokenizer, "primarykey") == 0) {
                        attribute->constraints->primary_key = true;
                        // TODO: test - this doesn't work -- returns null since tries
                        // to access spot in table that doesn't exist
                        add_primary_key_to_table(table, create_key(attribute->name, table));
                    } else if (strcasecmp(tokenizer, "unique") == 0) {
                        // TODO: add unique contraint to table
                        attribute->constraints->unique = true;
                    } else if (strcasecmp(tokenizer, "notnull") == 0) {
                        // flag attribute as notnull
                        attribute->constraints->notnull = true;
                    } else {
                        // else error since invalid token?
                        free(attribute->name);
                        free(attribute->constraints);
                        free(attribute);
                        return -1;
                    }
                }
            }
        }
        table->attributes = realloc(table->attributes,
                                    sizeof(struct Attribute) * (table->attribute_count + 1));
        table->attributes[table->attribute_count++] = attribute;

        // read in data type of attribute and add it to array of datatypes.
        table->data_types = realloc(table->data_types, sizeof(int) * (table->data_type_size + 1));
        table->data_types[table->data_type_size] = attribute->type;
        table->data_type_size++;
        return 0;
    }
    free(attribute);
    return -1;
}

void freeKey(PrimaryKey key) {
    free(key->attributes);
    free(key);
}

void freeTable(Table table) {
    free(table->name);
    free(table->data_types);

    // free primary key
    if (table->primary_key != NULL){
        freeKey(table->primary_key);
    }

    // free attributes
    for (int i = 0; i < table->attribute_count; i++) {
        free(table->attributes[i]->name);
        free(table->attributes[i]->constraints);

        if (table->attributes[i]->foreignKey != NULL) {
            free(table->attributes[i]->foreignKey->referenced_table_name);
            free(table->attributes[i]->foreignKey->referenced_column_name);
            free(table->attributes[i]->foreignKey);
        }

        free(table->attributes[i]);
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
PrimaryKey create_key(char *attribute_names, Table table) {
    PrimaryKey key = malloc(sizeof(struct PrimaryKey));
    key->attributes = NULL;
    key->size = 0;
    char *tokenizer = strtok(attribute_names, " "); // split the attributes on space
    while (tokenizer != NULL) {
        for (int i = 0; i < table->attribute_count; i++) {
            if (strcasecmp(tokenizer, table->attributes[i]->name) == 0) {
                key->attributes = realloc(key->attributes, sizeof(struct Attribute *) * (key->size + 1));
                key->attributes[key->size] = table->attributes[i];
                key->size++;
            }
        }
        tokenizer = strtok(NULL, " ");
    }
    return key;
}

int parseForeignKey(Table table, char *tokenizer, char **token) {
    printf("parsing foreign key: %s\n", tokenizer);
    char *attribute_names = NULL;
    char *referenced_table_name = NULL;
    char *referenced_table_attributes = NULL;

    // parse through rest of statement
    if (tokenizer != NULL) {
        attribute_names = malloc(sizeof(char) * strlen(tokenizer)); // copy the attribute names to memory
        strcpy(attribute_names, tokenizer); // tokenize this string

        // parse references
        tokenizer = strtok_r(NULL, " ()", token);

        if (tokenizer != NULL && strcasecmp(tokenizer, "references") == 0) {

            // read table name
            tokenizer = strtok_r(NULL, " ()", token);
            if (tokenizer != NULL) {
                referenced_table_name = malloc(strlen(tokenizer) + 1);
                strcpy(referenced_table_name, tokenizer);

                // read attributes
                tokenizer = strtok_r(NULL, "()", token);
                if (tokenizer != NULL) {
                    referenced_table_attributes = malloc(strlen(tokenizer) + 1);
                    strcpy(referenced_table_attributes, tokenizer);
//                    tokenizer = strtok_r(NULL, " (),", token);
                } else {
                    // no attributes specified
                    free(referenced_table_name);
                    free(attribute_names);
                    return -1;
                }
            } else {
                // no table name specified
                free(attribute_names);
                return -1;
            }
        } else {
            // wrong keyword (not 'reference')
            free(attribute_names);
            return -1;
        }
    }
    ///
    /// TODO THIS NEEDS TO BE TESTED.
    ///
    // since we were able to parse the information we needed, assign the foreign keys.
    if (attribute_names != NULL && referenced_table_name != NULL && referenced_table_attributes != NULL) {
        printf("attr names: %s, referenced table: %s, referenced attrs: %s\n", attribute_names, referenced_table_name, referenced_table_attributes);

        Table referenced_table = get_table_from_catalog(referenced_table_name);

        if (referenced_table != NULL) {
            char *attr_token; // token for tokenizing attribute names
            char *referenced_attr_token; // token used for tokenizing referenced attributes;

            char *attribute_tokenizer = strtok_r(attribute_names, " ", &attr_token);
            char *referenced_attribute_tokenizer = strtok_r(referenced_table_attributes, " ", &referenced_attr_token);

            // for each attribute
            while (attribute_tokenizer != NULL) {

                // search for the attribute in table
                for (int i = 0; i < table->attribute_count; i++) {
                    if (strcasecmp(attribute_tokenizer, table->attributes[i]->name) == 0) {

                        if (referenced_attribute_tokenizer != NULL) {
                            printf("referenced attribute: %s\n", referenced_attribute_tokenizer);

                            bool found_attr = false;

                            // Check if referenced attribute exists in the referenced table
                            for (int j = 0; j < referenced_table->attribute_count; j++) {
                                if (strcasecmp(referenced_attribute_tokenizer, referenced_table->attributes[j]->name) == 0) {
                                    found_attr = true;
                                }
                            }

                            if (!found_attr) {
                                // Value given to reference was not found in the table
                                return -1;
                            }

                            ForeignKey fkey = malloc(sizeof(struct ForeignKey));
                            char *ref_col = malloc(sizeof(char) * strlen(referenced_attribute_tokenizer));
                            char *ref_table = malloc(sizeof(char) * strlen(referenced_table_name));
                            memcpy(ref_col, referenced_attribute_tokenizer, strlen(referenced_attribute_tokenizer));
                            memcpy(ref_table, referenced_table_name, strlen(referenced_table_name));
                            fkey->referenced_column_name = ref_col;
                            fkey->referenced_table_name = ref_table;
                            table->attributes[i]->foreignKey = fkey;

                            printf("ref_col: %s, ref_table: %s\n", ref_col, ref_table);
                            referenced_attribute_tokenizer = strtok_r(NULL, " ", &referenced_attr_token);
                            // search through second table
                            /*
                            for (int j = 0; j < referenced_table->attribute_count; j++) {
                                if (strcasecmp(referenced_attribute_tokenizer, referenced_table->attributes[j]->name) ==
                                    0) {
                                    int key_count = table->attributes[i]->foreign_key_count;

                                    // add space in array
                                    table->attributes[i]->foreignKey = realloc(table->attributes[i]->foreignKey,
                                                                               sizeof(ForeignKey *) * (key_count + 1));

                                    // allocate space within array
                                    table->attributes[i]->foreignKey[key_count] = malloc(sizeof(struct ForeignKey));

                                    // copy string
                                    table->attributes[i]->foreignKey[key_count]->referenced_table_name = malloc(
                                            strlen(referenced_table_name) + 1);
                                    strcpy(table->attributes[i]->foreignKey[key_count]->referenced_table_name,
                                           referenced_table_name);

                                    // allocate space for key
                                    table->attributes[i]->foreignKey[key_count]->referenced_key = malloc(
                                            sizeof(struct Key));

                                    // copy and store primary key as foreign key within attribute
                                    table->attributes[i]->foreignKey[key_count]->referenced_key->key_indices = malloc(
                                            sizeof(int *) * referenced_table->primary_key->size);
                                    memcpy(table->attributes[i]->foreignKey[key_count]->referenced_key->key_indices,
                                           referenced_table->primary_key->key_indices,
                                           sizeof(int *) * referenced_table->primary_key->size);

                                    // set size of key
                                    table->attributes[i]->foreignKey[key_count]->referenced_key->size = referenced_table->primary_key->size;

                                    // update the key count within the attribute
                                    table->attributes[i]->foreign_key_count++;

                                    // update the total foreign key count
                                    table->foreign_key_count++;
                                }

                            }
                            */
                            attribute_tokenizer = strtok_r(NULL, " ", &attr_token);
                        }
                    }

                }
            }
            free(attribute_names);
            free(referenced_table_name);
            free(referenced_table_attributes);
            return 0;
        }
        // error since table doesn't exist
        free(attribute_names);
        free(referenced_table_name);
        free(referenced_table_attributes);
        return -1;
    }
    return -1;
}

// todo: test
int add_primary_key_to_table(Table table, PrimaryKey key) {
    if (table->primary_key_count == 0) {
//        table->key_indices = realloc(table->key_indices, (key->key_indices_count + 1) * sizeof(int *));
//        memcpy(table->key_indices, key->key_indices, key->key_indices_count * sizeof(int *));
//        table->primary_key_count++;
//        table->key_indices_count = key->key_indices_count;
//        free(key->key_indices);
//        free(key);
        table->primary_key = key;
        table->primary_key_count++;
        return 0;
    }
    freeKey(key);
    return -1; // error since primary key already exists in table.
}

int add_unique_key_to_table(Table table, PrimaryKey key) {
//    table->unique_keys = realloc(table->unique_keys, sizeof(Key) * (table->unique_key_count + 1));
//    table->unique_keys[table->unique_key_count] = key;
    return 0;
}

char* get_catalog_file_path() {
    // format the catalog file name and path
    char * catalog_file_name = "/catalog";
    char * catalog_path = malloc(strlen(global_db_loc) + strlen(catalog_file_name) + 1);
    strcpy(catalog_path, global_db_loc);
    strcat(catalog_path, catalog_file_name);

    return catalog_path;

}

int createCatalog(Table table) {

    catalog = malloc(sizeof(struct Catalog));

    // allocate memory for struct attributes
    catalog->tables = malloc(sizeof(struct Table *));

    // add initial table
    catalog->tables[0] = table;
    catalog->table_count = 1;
    return 0;
}

Table get_table_from_catalog(char *table_name) {
    if (catalog != NULL) {
        for (int i = 0; i < catalog->table_count; i++) {
            if (strcasecmp(catalog->tables[i]->name, table_name) == 0) {
                return catalog->tables[i];
            }
        }
    }
    return NULL;
}

int add_table_to_catalog(Table table) {
    if (catalog == NULL) {
        return createCatalog(table);
    }
    catalog->tables = realloc(catalog->tables, sizeof(Table) * (catalog->table_count + 1));
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
            catalog->tables[i] = catalog->tables[i + 1];
        }

        // Success
        return 0;
    } else {
        // Table was not found in list
        return -1;
    }
}

struct Table *createTable(char *name) {
    struct Table *table_data = malloc(sizeof(struct Table));

    // set counts and allocate memory
    table_data->attribute_count = 0;
    table_data->primary_key_count = 0;
    table_data->key_indices_count = 0;
    table_data->data_type_size = 0;
    table_data->attributes = malloc(sizeof(struct Attribute));
    table_data->data_types = malloc(sizeof(int));
    table_data->primary_key = NULL;
    table_data->name = malloc(strlen(name) + 1);
    table_data->name_size = strlen(name) + 1;
    strcpy(table_data->name, name);

    return table_data;
}

void freeCatalog() {
    for (int i = 0; i < catalog->table_count; i++) {
        freeTable(catalog->tables[i]);
    }
    free(catalog->tables);
    free(catalog);
}

int write_primary_key_to_disk(FILE *file, struct PrimaryKey *primaryKey) {
    // might not be needed if size 0 and null are treated the same
    if(primaryKey == NULL){
        fwrite(&primaryKey, sizeof(int), 1, file);
    }
    else {
        fwrite(&primaryKey->size, sizeof(int), 1, file);
        // write each attribute
        for(int i = 0; i < primaryKey->size; i++) {
            write_attribute_to_disk(file, primaryKey->attributes[i]);
        }
    }
    return 0;
}

int write_foreign_key_to_disk(FILE *file, struct ForeignKey *foreignKey) {
    // write the corresponding table and column name
    fwrite(foreignKey->referenced_table_name, sizeof(foreignKey->referenced_table_name), 1, file);
    fwrite(foreignKey->referenced_table_name, sizeof(foreignKey->referenced_column_name), 1, file);
    
    return 0;
}

int write_attribute_to_disk(FILE *file, struct Attribute *attribute) {
    fwrite(&attribute->name_size, sizeof(int), 1, file);
    fwrite(attribute->name, sizeof(attribute->name), 1, file);
    
    // read size of arrays
    fwrite(&attribute->type, sizeof(int), 1, file);
    fwrite(&attribute->size, sizeof(int), 1, file);
    fwrite(&attribute->name_size, sizeof(int), 1, file);
    
    // write constaints
    fwrite(attribute->constraints, sizeof(struct Constraints), 1, file);
    // write foreign key
    write_foreign_key_to_disk(file, attribute->foreignKey);

    return 0;
}

int write_table_to_disk(FILE *file, struct Table *table) {
    // write id
    printf("table_id: %d\n", table->tableId);
    fwrite(&table->tableId, sizeof(int), 1, file);
    
    // write name
    printf("name_size: %d\n", table->name_size);
    fwrite(&table->name_size, sizeof(int), 1, file);
    printf("name: %s\n", table->name);
    fwrite(table->name, sizeof(table->name), 1, file);
    
    // write array sizes
    fwrite(&table->primary_key_count, sizeof(int), 1, file);
    
    //fwrite(&table->foreign_key_count, sizeof(int), 1, file);
    fwrite(&table->attribute_count, sizeof(int), 1, file);
    fwrite(&table->key_indices_count, sizeof(int), 1, file);
    fwrite(&table->data_type_size, sizeof(int), 1, file);
    
    // write each attribute
    for(int i = 0; i < table->attribute_count; i++) {
        write_attribute_to_disk(file, table->attributes[i]);
    }
    // write data types array
    fwrite(table->data_types, sizeof(int), table->data_type_size, file);
    
    // write primary key
    write_primary_key_to_disk(file, table->primary_key);

    return 0;
}

int write_catalog_to_disk() {
    char * catalog_path = get_catalog_file_path();

    // catalog data
    int table_count = catalog->table_count;
    struct Table **tables = catalog->tables;

    // open file and write the catalog data
    printf("Writing catalog to disk: %s\n", catalog_path);
    FILE *catalog_file = fopen(catalog_path, "wb");

    // write table count
    printf("table_count: %d\n", table_count);
    fwrite(&table_count, sizeof(int), 1, catalog_file);
    

    // write each table struct and all its data
    for(int i = 0; i < catalog->table_count; i++){
        write_table_to_disk(catalog_file, tables[i]);
    }

    fclose(catalog_file);
    free(catalog_path);
    return 0;
}

struct PrimaryKey* read_primary_key_from_disk(FILE *file) {
    int key_size;
    fread(&key_size, sizeof(int), 1, file);
    if(key_size == 0){
        return NULL;
    }
    else {
        struct PrimaryKey *key = malloc(sizeof(int) + sizeof(int) * key_size);
        //fread(&key->size, sizeof(int), 1, file);
        //fread(key->key_indices, sizeof(int), key->size, file);
        return key;
    }
}

struct Table* read_table_from_disk(FILE *file) {
    int tableID;
    int name_size;
    char *name = malloc(name_size);

    fread(&tableID, sizeof(int), 1, file);
    fread(&name_size, sizeof(int), 1, file);
    fread(name, name_size, 1, file);

    // only adds name and name size to struct
    struct Table* table = createTable(name);
    table->tableId = tableID;
    free(name);

    // write array sizes
    fread(&table->primary_key_count, sizeof(int), 1, file);
    //fread(&table->foreign_key_count, sizeof(int), 1, file);
    fread(&table->attribute_count, sizeof(int), 1, file);
    //fread(&table->unique_key_count, sizeof(int), 1, file);
    fread(&table->data_type_size, sizeof(int), 1, file);

    // write primary key
    table->primary_key = read_primary_key_from_disk(file);
    // write each attribute
    for(int i = 0; i < table->attribute_count; i++) {
        //table->attributes[i] = read_attribute_to_disk(file);
    }
    // write each unique key
    /*
    for(int i = 0; i < table->unique_key_count; i++) {
        //read_key_to_disk(file, table->unique_keys[i]);
    }
    */

    return 0;
}

int read_catalog_from_disk() {
    char * catalog_path = get_catalog_file_path();

    // open file and read the catalog data
    printf("Reading catalog from disk: %s\n", catalog_path);
    FILE *catalog_file = fopen(catalog_path, "rb");

    // read table count
    int table_count;
    fread(&table_count, sizeof(int), 1, catalog_file);

    // read each table struct and all its data
    for(int i = 0; i < table_count; i++){
        read_table_from_disk(catalog_file);
    }

    fclose(catalog_file);
    free(catalog_path);
    return 0;
}