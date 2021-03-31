/**
 * CSCI-421 Project: Phase1
 * @file ddl_manager.c
 * Description:
 *      This file contains the function definitions defined in '../headers/ddl_manager.h'.
 *      This file handles the core functionality of ddl parsing
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */

#include "../headers/ddl_manager.h"
#include "../headers/storagemanager.h"
#include "../headers/catalog.h"
#include <float.h>

int initialize_ddl_parser(char *db_loc, bool restart) {
    GLOBAL_DB_LOCATION = malloc(strlen(db_loc) + 1);
    strcpy(GLOBAL_DB_LOCATION, db_loc);

//    printf("Starting DDL parser: %s\n", global_db_loc);

    if (restart) {
        read_catalog_from_disk(); // this has yet to be defined
    };
    return 0;
}

int terminate_ddl_parser() {
    int result = 0;
    result = write_catalog_to_disk();
    free(GLOBAL_DB_LOCATION);
    terminate_database();
    return result;
}

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

int parseDrop(char *tokenizer, char **token) {
    tokenizer = strtok_r(NULL, " ", token);
    // table <name>
    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {
        //("%s\n", tokenizer);

        // read in the table name
        tokenizer = strtok_r(NULL, " ", token);

        if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
            //printf("%s\n", tokenizer);

            // read table from catalog
            Table table = get_table_from_catalog(tokenizer);

            // drop the table in storagemanager.
            if (table != NULL && drop_table(table->tableId) == 0) {

                // update the catalog to remove reference of table
                remove_table_from_catalog(table->name);

                // iterate through tables
                for (int i = 0; i < catalog->table_count; i++) {

                    if (catalog->tables[i] != NULL) {

                        // iterate though all attributes
                        for (int j = 0; j < catalog->tables[i]->attribute_count; j++) {
                            if (catalog->tables[i]->attributes[j]->foreignKey != NULL) {
                                // free foreign key
                                if (strcasecmp(catalog->tables[i]->attributes[j]->foreignKey->referenced_table_name,
                                               table->name) == 0) {
                                    free(catalog->tables[i]->attributes[j]->foreignKey->referenced_table_name);
                                    free(catalog->tables[i]->attributes[j]->foreignKey->referenced_column_name);
                                    free(catalog->tables[i]->attributes[j]->foreignKey);
                                    catalog->tables[i]->attributes[j]->foreignKey = NULL;
                                }
                            }
                        }

                    }
                }

                // free the memory within the heap
                freeTable(table);
                catalog->table_count--;
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

int parseAlter(char *tokenizer, char **token) {
    tokenizer = strtok_r(NULL, " ", token);

    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {
//        printf("%s\n", tokenizer);

        // read in the table name
        tokenizer = strtok_r(NULL, " ", token);
        //char *table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
        char *table_name;
        //printf("length right now: %zu\n", strlen(tokenizer));

        if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
//            printf("table name: %s\n", tokenizer);

            // Set table name to be used later
            // TODO free this at every return point
            table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
            strcpy(table_name, tokenizer);
            // read in function type
            tokenizer = strtok_r(NULL, " ", token);

            Table table_to_alter = get_table_from_catalog(table_name);

            // Error if table is not in the catalog
            if (table_to_alter == NULL) {
                fprintf(stderr, "Error: Given table does not exist\n");
                free(table_name);
                return -1;
            }

            //drop <name>
            if (tokenizer != NULL && strcasecmp(tokenizer, "drop") == 0) {
//                printf("alter operation: %s\n", tokenizer);

                // read attribute name
                tokenizer = strtok_r(NULL, " ", token);

                if (tokenizer != NULL && strcasecmp(tokenizer, "") != 0) {
//                    printf("attribute name: %s\n", tokenizer);

                    //char *table_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    //strcpy(table_name, tokenizer);

                    // read table from catalog

//                    printf("Found table in catalog, has %d attributes\n", table_to_alter->attribute_count);

                    for (int i = 0; i < table_to_alter->attribute_count; i++) {
//                        printf("looking for %s, have %s\n", tokenizer, table_to_alter->attributes[i]->name);
                        if (strcasecmp(table_to_alter->attributes[i]->name, tokenizer) == 0) {
                            // Found correct attribute to drop
                            Attribute dropping = table_to_alter->attributes[i];
//                            printf("Found correct attribute to drop\n");

                            // Check if attribute is part of primary key
                            for (int j = 0; j < table_to_alter->primary_key->size; j++) {

//                                printf("comparing %s to %s in primarykey\n", dropping->name, table_to_alter->primary_key->attributes[j]->name);
                                if (strcmp(table_to_alter->primary_key->attributes[j]->name, dropping->name) == 0) {
                                    fprintf(stderr, "Error: cannot drop attribute in primary key\n");
                                    free(table_name);
                                    return -1;
                                }
                            }

                            // Search for references to the attribute in foreign keys of all other tables
                            for (int j = 0; j < catalog->table_count; j++) {
                                // Go through attributes and check if they have a foreign key
                                for (int k = 0; k < catalog->tables[j]->attribute_count; k++) {
                                    Attribute checking = catalog->tables[j]->attributes[k];
                                    if (checking->foreignKey != NULL
                                        &&
                                        strcasecmp(table_to_alter->name, checking->foreignKey->referenced_table_name) ==
                                        0
                                        &&
                                        strcasecmp(dropping->name, checking->foreignKey->referenced_column_name) == 0) {
                                        // This attribute referenced the table and attribute we were given,
                                        // so need to drop that foreign key as we're dropping the attribute
                                        checking->foreignKey = NULL;
//                                        printf("Removed foreign key from attribute %s in table %s\n", checking->name, catalog->tables[j]->name);
                                    }
                                }
                            }

                            freeAttribute(dropping);

                            // Remove the attribute from the list of attributes
                            // Set all values in Table struct to no longer include that attribute
                            for (int move = i; move < table_to_alter->attribute_count - 1; move++) {
                                table_to_alter->attributes[move] = table_to_alter->attributes[move + 1];
                                table_to_alter->data_types[move] = table_to_alter->data_types[move + 1];
                            }

                            table_to_alter->attribute_count--;
                            table_to_alter->data_type_size--;

                            // ===== storagemanager.c stuff =======
                            // What needs to change:
                            // tuples_per_page, num_attr, attr_types

                            union record_item **records = NULL;
                            int num_records = get_records(table_to_alter->tableId, &records);
//                            printf("got %d records\n", num_records);

                            drop_table(table_to_alter->tableId);
                            table_to_alter->tableId = add_table(table_to_alter->data_types, table_to_alter->key_indices,
                                                                table_to_alter->data_type_size,
                                                                table_to_alter->key_indices_count);

                            for (int j = 0; j < num_records; j++) {
                                for (int k = i; k < table_to_alter->data_type_size - 1; k++) {
                                    records[j][k] = records[j][k + 1];
                                }
                                insert_record(table_to_alter->tableId, records[j]);
                            }

                            free(table_name);
                            return 0;
                        }
                    }
                    // update the table
                    // save the table to catalog

                    free(table_name);
                    fprintf(stderr, "Error: Attribute does not exist in table\n");
                    return -1; // correct structure
                }
                return -1; // incorrect syntax/structure
            }

                // add <a_name> <a_type>
                // add <a_name> <a_type> default <value>
            else if (tokenizer != NULL && strcasecmp(tokenizer, "add") == 0) {
//                printf("%s\n", tokenizer);
                // read <a_name>
                tokenizer = strtok_r(NULL, " ", token);
                if (tokenizer != NULL) {
//                    printf("%s\n", tokenizer);
                    char *a_name = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                    char *a_type = NULL;
                    char *a_size = NULL;
                    char *_default = NULL;
                    char *value = NULL;

                    if (is_valid_name(tokenizer) == -1) {
                        free(a_name);
                        free(table_name);
                        return -1;
                    }

                    strcpy(a_name, tokenizer);
//                    printf("name should be %s\n", a_name);

                    // read <a_type>
                    tokenizer = strtok_r(NULL, " (", token);

                    if (tokenizer != NULL) {
//                        printf("a_type: %s\n", tokenizer);
                        a_type = malloc(sizeof(char) * (strlen(tokenizer) + 1));
                        strcpy(a_type, tokenizer);
                        int check_type = get_attribute_type(a_type);
                        if (check_type == 3 || check_type == 4) {
//                            printf("found char or varchar\n");
                            tokenizer = strtok_r(NULL, " ()", token);
                            a_size = malloc(sizeof(char) * (strlen(tokenizer) + 1));
                            strcpy(a_size, tokenizer);
//                            printf("got size of %s\n", a_size);
                        }

                        // read default (this is optional)
                        tokenizer = strtok_r(NULL, " ", token);

                        if (tokenizer != NULL && strcasecmp(tokenizer, "default") == 0) {
//                            printf("%s\n", tokenizer);
                            _default = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                            strcpy(_default, tokenizer);

                            // read <value>
                            tokenizer = strtok_r(NULL, " ", token);

                            if (tokenizer != NULL) {
//                                printf("%s\n", tokenizer);
                                value = malloc(sizeof(char *) * strlen(tokenizer) + 1);
                                strcpy(value, tokenizer);

                                // Malloc new attribute
                                Attribute new_attr = malloc(sizeof(struct Attribute));
                                // Fill out to match the type
                                new_attr->name = malloc(sizeof(char) * (strlen(a_name) + 1));
                                strcpy(new_attr->name, a_name);
//                                printf("name is now %s\n", new_attr->name);
                                new_attr->name_size = strlen(a_name) + 1;
                                new_attr->type = get_attribute_type(a_type);
                                if (new_attr->type == 3 || new_attr->type == 4) {
                                    // TODO handle char/varchar
                                    int size = strtol(a_size, NULL, 10);
                                    if (size >= 0) {
                                        new_attr->size = size;
                                    } else {
                                        fprintf(stderr, "Error: invalid size parameter for char or varchar\n");
                                        free(a_type);
                                        free(a_name);
                                        free(a_size);
                                        return -1;
                                    }
                                } else {
                                    // zero out size field
                                    new_attr->size = 0;
                                }

                                new_attr->foreignKey = NULL;

                                // Create constraints struct
                                new_attr->constraints = malloc(sizeof(struct Constraints));
                                new_attr->constraints->notnull = false;
                                new_attr->constraints->primary_key = false;
                                new_attr->constraints->unique = false;

                                // Add to end of attributes list (realloc)
                                table_to_alter->attributes = realloc(table_to_alter->attributes, sizeof(Attribute) *
                                                                                                 (table_to_alter->attribute_count +
                                                                                                  1));
                                table_to_alter->attributes[table_to_alter->attribute_count++] = new_attr;
                                // Add stuff to end of data_types (also realloc)
                                table_to_alter->data_types = realloc(table_to_alter->data_types, sizeof(int) *
                                                                                                 (table_to_alter->data_type_size +
                                                                                                  1));
                                table_to_alter->data_types[table_to_alter->data_type_size++] = new_attr->type;

                                // Get records from storagemanager
                                union record_item **records = NULL;
                                int num_records = get_records(table_to_alter->tableId, &records);
                                // drop old table
                                drop_table(table_to_alter->tableId);
                                // Create new table, set tableId
                                table_to_alter->tableId = add_table(table_to_alter->data_types,
                                                                    table_to_alter->key_indices,
                                                                    table_to_alter->data_type_size,
                                                                    table_to_alter->key_indices_count);

                                // For each record, realloc them to be one bigger and slap the default value on the end
                                for (int i = 0; i < num_records; i++) {
                                    records[i] = realloc(records[i],
                                                         sizeof(union record_item) * table_to_alter->data_type_size);
                                    switch (new_attr->type) {
                                        case 0:
                                            records[i][table_to_alter->data_type_size - 1].i = strtol(value, NULL, 10);
                                            break;
                                        case 1:
                                            records[i][table_to_alter->data_type_size - 1].d = strtod(value, NULL);
                                            break;
                                        case 2:
                                            if (strcasecmp(value, "true") == 0) {
                                                records[i][table_to_alter->data_type_size - 1].b[0] = true;
                                                // Setting this byte to 1 means the value is not null
                                                records[i][table_to_alter->data_type_size - 1].b[1] = 1;
                                            } else if (strcasecmp(value, "false") == 0) {
                                                records[i][table_to_alter->data_type_size - 1].b[0] = false;
                                                records[i][table_to_alter->data_type_size - 1].b[1] = 1;
                                            } else {
                                                free(table_name);
                                                free(_default);
                                                fprintf(stderr, "Error: invalid value for boolean attribute\n");
                                                return -1;
                                            }
                                            break;
                                        case 3:
                                            strcpy(records[i][table_to_alter->data_type_size - 1].c, value);
                                            break;
                                        case 4:
                                            strcpy(records[i][table_to_alter->data_type_size - 1].v, value);
                                            break;
                                        default:
                                            free(table_name);
                                            free(_default);
                                            fprintf(stderr, "Error: invalid attribute type\n");
                                            return -1;
                                    }
                                    // then insert them into the new table
                                    insert_record(table_to_alter->tableId, records[i]);
                                }

                                free(value);
                                free(_default);
                                free(a_type);
                                free(a_name);
                                free(a_size);
                                return 0; // proper command structure.
                            }

                            free(_default); // error since no default value specified.
                        }

                        // Malloc new attribute
                        Attribute new_attr = malloc(sizeof(struct Attribute));

                        // Fill out to match the type
                        new_attr->name = malloc(sizeof(char) * (strlen(a_name) + 1));
                        strcpy(new_attr->name, a_name);
                        new_attr->name_size = strlen(a_name) + 1;
                        new_attr->type = get_attribute_type(a_type);
//                        printf("got type %d\n", new_attr->type);

                        if (new_attr->type == 3 || new_attr->type == 4) {
                            // TODO handle char/varchar
                            int size = strtol(a_size, NULL, 10);
                            if (size >= 0) {
                                new_attr->size = size;
                            } else {
                                fprintf(stderr, "Error: invalid size parameter for char or varchar\n");
                                free(a_type);
                                free(a_name);
                                free(a_size);
                            }
                        } else {
                            // zero out size field
                            new_attr->size = 0;
                        }
                        new_attr->foreignKey = NULL;

                        // Create constraints struct
                        new_attr->constraints = malloc(sizeof(struct Constraints));
                        new_attr->constraints->notnull = false;
                        new_attr->constraints->primary_key = false;
                        new_attr->constraints->unique = false;
                        // Add to end of attributes list (realloc)
                        table_to_alter->attributes = realloc(table_to_alter->attributes,
                                                             sizeof(Attribute) * (table_to_alter->attribute_count + 1));
                        table_to_alter->attributes[table_to_alter->attribute_count++] = new_attr;
                        // Add stuff to end of data_types (also realloc)
                        table_to_alter->data_types = realloc(table_to_alter->data_types,
                                                             sizeof(int) * (table_to_alter->data_type_size + 1));
                        table_to_alter->data_types[table_to_alter->data_type_size++] = new_attr->type;

                        // Get records from storagemanager
                        union record_item **records = NULL;
                        int num_records = get_records(table_to_alter->tableId, &records);
                        // drop old table
                        drop_table(table_to_alter->tableId);
                        // Create new table, set tableId
                        table_to_alter->tableId = add_table(table_to_alter->data_types, table_to_alter->key_indices,
                                                            table_to_alter->data_type_size,
                                                            table_to_alter->key_indices_count);

                        // For each record, realloc them to be one bigger and set the value to null
                        // TODO figure out null values
                        for (int i = 0; i < num_records; i++) {
                            records[i] = realloc(records[i],
                                                 sizeof(union record_item) * table_to_alter->data_type_size);
                            // When read as any of the data types, this should behave like a null value
                            // TODO actually set this to a null value instead of zero
                            switch (new_attr->type) {
                                case 0:
                                    records[i][table_to_alter->data_type_size - 1].i = INT_MIN;
                                    break;
                                case 1:
                                    records[i][table_to_alter->data_type_size - 1].d = -DBL_MAX;
                                    break;
                                case 2:
                                    records[i][table_to_alter->data_type_size - 1].b[0] = false; // zero
                                    // If this bit is set to 0, value is null
                                    records[i][table_to_alter->data_type_size - 1].b[1] = 0;
                                    break;
                                case 3:
                                    // Make sure this is an empty string, everything after doesn't matter
                                    strcpy(records[i][table_to_alter->data_type_size - 1].c, "\0\0");
                                    break;
                                case 4:
                                    strcpy(records[i][table_to_alter->data_type_size - 1].v, "\0\0");
                                    break;
                                default:
                                    free(a_type);
                                    free(a_name);
                                    free(a_size);
                                    fprintf(stderr, "Error: invalid type given for new attribute\n");
                                    return -1;
                            }
                            // then insert them into the new table
                            insert_record(table_to_alter->tableId, records[i]);
                        }

                        free(a_type); // since no type specified.
                        free(a_name);
                        free(a_size);
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

    tokenizer = strtok_r(NULL, " ", token);

    if (tokenizer != NULL && strcasecmp(tokenizer, "table") == 0) {

        // read in Table name
        tokenizer = strtok_r(NULL, " (", token);
        if (tokenizer != NULL) {

            if (is_valid_name(tokenizer) == -1) {
                return -1;
            }

            if (catalog != NULL) {
                for (int i = 0; i < catalog->table_count; i++) {
                    if (strcasecmp(catalog->tables[i]->name, tokenizer) == 0) {
                        fprintf(stderr, "Error: table names must be unique\n");
                        return -1;
                    }
                }
            }

            // adding table to catalog will create its ID
            struct Table *table_data = createTable(tokenizer);

            // perform loop to read in rest of attributes and populate Table struct
            while (tokenizer != NULL) {
                tokenizer = strtok_r(NULL, ",", token);

                // this can potentially be null or empty space -> prevent any potential segfaults for bad reads
                if (tokenizer != NULL && strcmp(tokenizer, " ") != 0) {

                    // Cut off any leading spaces
                    char *filtered = malloc(strlen(tokenizer) + 1);
                    // Because we're incrementing filtered, save where the block of memory starts
                    char *save = filtered;
                    strcpy(filtered, tokenizer);

                    while (filtered[0] == ' ') {
                        filtered++;
                    }

                    int last = strlen(filtered) - 1;
                    while (filtered[last] == ' ') {
                        filtered[last] = '\0';
                        last--;
                    }

                    char *first_word = strtok(filtered, " ()");

                    // check for any keywords
                    if (strcasecmp(first_word, "primarykey") == 0) {
                        // reset tokenizer to be primarykey(1 2 ... n)//
                        tokenizer = strtok(tokenizer, "()");

                        // read in attribute names.
                        tokenizer = strtok(NULL, "()");
                        parsePrimaryKey(table_data, tokenizer);
                        tokenizer = first_word; // set to not null to allow for any further parsing potential
                    } else if (strcasecmp(first_word, "foreignkey") == 0) {
                        // reset tokenizer to be foreign(1 2 ... n)
                        tokenizer = strtok(tokenizer, "()");

                        // read in attribute names
                        tokenizer = strtok(NULL, "()");
                        parseForeignKey(table_data, tokenizer, token);
                        tokenizer = first_word; // set to not null to allow for any further parsing potential

                    } else if (strcasecmp(first_word, "unique") == 0) {
                        // read in attribute names
                        tokenizer = strtok(tokenizer, "()");

                        // read in attribute names.
                        tokenizer = strtok(NULL, "()");
                        parseUnique(table_data, tokenizer);
                    } else { // parse through the attributes/column information
                        int result;

                        // parse through string to check whether or not there is a char varchar keyword.
                        // if there is, rewrite string to be: 'foo char 5 notnull ... unique'
                        //                                 or 'foo varchar 5 notnull ... unique', etc.
                        first_word = strtok(NULL, " ");
                        if (char_or_varchar(first_word) == 0) {
                            for (int i = 0; i < strlen(tokenizer); i++) {
                                if (tokenizer[i] == '(' || tokenizer[i] == ')') {
                                    tokenizer[i] = ' ';
                                }
                            }
                        }
                        result = parseAttributes(table_data, tokenizer);

                        if (result == -1) {
                            free(save);
                            freeTable(table_data);
                            return -1;
                        }
                    }
                    free(save);
                }
            }
            table_data->tableId = add_table(table_data->data_types, table_data->key_indices, table_data->data_type_size,
                                            table_data->key_indices_count);
            add_table_to_catalog(table_data);
            return 0;
        }
    }
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
}

int parsePrimaryKey(Table table, char *names) {
    return add_primary_key_to_table(table, create_key(names, table));
}

int parseUnique(Table table, char *names_list) {
    if (table == NULL) {
        return -1;
    }
    char *name = strtok(names_list, " ");
    while (name != NULL) {
        // read in name
        for (int i = 0; i < table->attribute_count; i++) {
            if (strcasecmp(table->attributes[i]->name, name) == 0) {
                table->attributes[i]->constraints->unique = true;
            }
        }
        name = strtok(NULL, " ");
    }
    return 0;
}

int parseAttributes(Table table, char *tokenizer) {

    // Change in parsing means leading paren needs to be sliced off
    while (tokenizer[0] == ' ' || tokenizer[0] == '(') {
        tokenizer++;
    }
    int last = strlen(tokenizer) - 1;
    while (tokenizer[last] == ' ' || tokenizer[last] == ')') {
        tokenizer[last] = '\0';
        last--;
    }
//    printf("Attributes: %s\n", tokenizer);

    char *temp_token;
    tokenizer = strtok_r(tokenizer, " ", &temp_token); // split the string via spaces
    Attribute attribute = malloc(sizeof(struct Attribute));
//    printf("tokenizer now: %s\n", tokenizer);

    // read in attribute/ column name
    if (tokenizer != NULL) {
        attribute->name = malloc(strlen(tokenizer) + 1);
        attribute->name_size = strlen(tokenizer) + 1;
        attribute->type = 0;
        attribute->foreignKey = NULL;
        attribute->constraints = malloc(sizeof(struct Constraints));
        struct Constraints *constraints = attribute->constraints;
        attribute->default_size = 0;
        attribute->default_value = NULL; //todo:free
        constraints->primary_key = false;
        constraints->notnull = false;
        constraints->unique = false;

        if (is_valid_name(tokenizer) == -1) {
            free(attribute->name);
            free(attribute->constraints);
            free(attribute);
            return -1;
        }

        strcpy(attribute->name, tokenizer);
//        printf("name: %s\n", attribute->name);
        //printf("name_size: %d\n", attribute->name_size);

        // read in attribute type
        tokenizer = strtok_r(NULL, " ()", &temp_token);

        if (tokenizer != NULL) {
            // read in column type, function returns 0-4 based on string name (integer-varchar)
            attribute->type = get_attribute_type(tokenizer);
//            printf("type: %d\n", attribute->type);
            // check for correct attribute
            if (attribute->type == -1) {
                free(attribute);
                return -1;
            } else if (attribute->type == 3 || attribute->type == 4) {
                // parse size of string in base token.

                tokenizer = strtok_r(NULL, " ", &temp_token);
//                printf("%s\n\n", attribute->name);
                attribute->size = atoi(tokenizer);
//                temp_token = *token;
//                tokenizer = strtok_r(NULL, " ,)", token);
            } else {
                attribute->size = 0;
            }

            //printf("size: %d\n\n", attribute->size);

            // loop through constraints
            while (tokenizer != NULL) {

                tokenizer = strtok_r(NULL, " ,)", &temp_token);

                if (tokenizer != NULL) {

                    if (strcasecmp(tokenizer, "primarykey") == 0) {
                        if (table->primary_key_count > 0) {
                            fprintf(stderr, "Error: Cannot use primarykey constraint on multiple attributes\n");
                            free(attribute->name);
                            free(attribute->constraints);
                            free(attribute);
                            return -1;
                        }
                        constraints->primary_key = true;
                        struct PrimaryKey *key = create_key_from_attr(attribute, table);
                        add_primary_key_to_table(table, key);
                    } else if (strcasecmp(tokenizer, "unique") == 0) {
                        constraints->unique = true;
                    } else if (strcasecmp(tokenizer, "notnull") == 0) {
//                        printf("got nontnull\n");
                        constraints->notnull = true;
                    } else if (strcasecmp(tokenizer, "default") == 0) {
                        // read value of default
                        tokenizer = strtok_r(NULL, " ,)", &temp_token);
                        if (tokenizer != NULL) {
                            // store the value as a string, convert value later on when needed
                            attribute->default_value = malloc(strlen(tokenizer) + 1);
                            strcpy(attribute->default_value, tokenizer);
                            attribute->default_size = strlen(tokenizer) + 1;
                        }
                    } else { // bad value
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

int parseForeignKey(Table table, char *tokenizer, char **token) {
//    printf("parsing foreign key: %s\n", tokenizer);
    char *attribute_names = NULL;
    char *referenced_table_name = NULL;
    char *referenced_table_attributes = NULL;

    // parse through rest of statement
    if (tokenizer != NULL) {
        attribute_names = malloc(sizeof(char) * strlen(tokenizer) + 1); // copy the attribute names to memory
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

    // since we were able to parse the information we needed, assign the foreign keys.
    if (attribute_names != NULL && referenced_table_name != NULL && referenced_table_attributes != NULL) {
//        printf("attr names: %s, referenced table: %s, referenced attrs: %s\n", attribute_names, referenced_table_name, referenced_table_attributes);

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

//                    printf("comparing %s to %s\n", attribute_tokenizer, table->attributes[i]->name);
                    if (strcasecmp(attribute_tokenizer, table->attributes[i]->name) == 0) {

                        if (referenced_attribute_tokenizer != NULL) {
//                            printf("referenced attribute: %s\n", referenced_attribute_tokenizer);

                            bool found_attr = false;

                            // Check if referenced attribute exists in the referenced table
                            for (int j = 0; j < referenced_table->attribute_count; j++) {
                                if (strcasecmp(referenced_attribute_tokenizer, referenced_table->attributes[j]->name) ==
                                    0) {
                                    found_attr = true;
                                }
                            }

                            if (!found_attr) {
                                // Value given to reference was not found in the table
                                fprintf(stderr, "Error: Cannot find value to reference\n");
                                return -1;
                            }

                            ForeignKey fkey = malloc(sizeof(struct ForeignKey));
                            char *ref_col = malloc(strlen(referenced_attribute_tokenizer) + 1);
                            char *ref_table = malloc(strlen(referenced_table_name) + 1);
                            memcpy(ref_col, referenced_attribute_tokenizer, strlen(referenced_attribute_tokenizer) + 1);
                            memcpy(ref_table, referenced_table_name, strlen(referenced_table_name) + 1);
                            fkey->referenced_column_name = ref_col;
                            fkey->referenced_column_name_size = strlen(referenced_attribute_tokenizer) + 1;
                            fkey->referenced_table_name = ref_table;
                            fkey->referenced_table_name_size = strlen(referenced_table_name) + 1;
                            table->attributes[i]->foreignKey = fkey;

//                            printf("ref_col: %s, ref_table: %s\n", ref_col, ref_table);
                            referenced_attribute_tokenizer = strtok_r(NULL, " ", &referenced_attr_token);
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
        fprintf(stderr, "Error: could not find referenced table\n");
        // error since table doesn't exist
        free(attribute_names);
        free(referenced_table_name);
        free(referenced_table_attributes);
        return -1;
    }
    return -1;
}

int char_or_varchar(char *word) {
    // char
    if ((word[0] == 'c' || word[0] == 'C') && (word[1] == 'h' || word[1] == 'H') && (word[2] == 'a' || word[2] == 'A')
        && (word[3] == 'r' || word[3] == 'R')) {
        return 0;
    }
    // varchar
    else if ((word[0] == 'v' || word[0] == 'V') && (word[1] == 'a' || word[1] == 'A') &&
             (word[2] == 'r' || word[1] == 'R')
             && (word[3] == 'c' || word[3] == 'C') && (word[4] == 'h' || word[4] == 'H') &&
             (word[5] == 'a' || word[5] == 'A')
             && (word[6] == 'r' || word[6] == 'R')) {
        return 0;
    }
    return -1;
}

int is_valid_name(char *name) {
    if (name == NULL) {
        return -1;
    }

    if (name[0] < 65 || name[0] > 90) {
        if (name[0] < 97 || name[0] > 122) {
            fprintf(stderr, "Error: names must start with a letter\n");
            return -1;
        }
    }

    // First character is a lowercase or uppercase letter

    for (int i = 1; i < strlen(name); i++) {
        if (name[i] < 48 || (name[i] > 57 && name[i] < 65) || (name[i] > 90 && name[i] < 97) || name[i] > 122) {
            fprintf(stderr, "Error: names must contain only alphanumeric characters\n");
            return -1;
        }
    }

    char keywords[16][11] = {"create", "alter", "drop", "add", "table", "integer", "double", "bool", "char", "varchar",
                             "primarykey", "foreignkey", "unique", "default", "notnull", "references"};

    for (int i = 0; i < 16; i++) {
        if (strcasecmp(keywords[i], name) == 0) {
            fprintf(stderr, "Error: %s is a restricted keyword and cannot be used as a name\n", keywords[i]);
            return -1;
        }
    }

    return 0;
}
