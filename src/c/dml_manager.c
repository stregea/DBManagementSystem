#include "../headers/dml_manager.h"
#include "../headers/Enums.h"
#include "../headers/catalog.h"
#include "../headers/clause_parser.h"
#include "../headers/storagemanager.h"
#include "../headers/arrays.h"
#include "../headers/tuple.h"
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/**
 * Retrieve the index of the first occurrence of a word within a string.
 * @param str - The string to search.
 * @param word - The word to search for.
 * @return -1 on error, otherwise the index at which the word first occurs.
 */
int get_index_of_word_from_string(const char *string, const char *word) {
    int i, j, flag;

    i = 0;
    flag = 0;
    while (string[i] != '\0') {
        if (string[i] == word[0]) {
            flag = 1;
            j = 0;
            while (word[j] != '\0') {
                if (string[i + j] != word[j]) {
                    flag = 0;
                    break;
                }
                j++;
            }
        }
        if (flag == 1) {
            break;
        }
        i++;
    }
    if (flag == 0) {
        return -1;
    } else {
        return i;
    }
}

void freeRecord(union record_item *record) {
    if (record != NULL) {
        free(record);
    }
}

/**
 * Print out a record.
 */
void print_record(Table table, union record_item *record) {
    printf("(");
    for (int i = 0; i < table->attribute_count; i++) {
        char *extra_space = " ";

        if (i == table->attribute_count - 1) {
            extra_space = ")\n"; // close the tuple, create new line
        }
        switch (table->attributes[i]->type) {
            case INTEGER:
                printf("%d%s", record[i].i, extra_space);
                break;
            case DOUBLE:
                printf("%f%s", record[i].d, extra_space);
                break;
            case BOOL:
                printf("%d%s", record[i].b[0], extra_space); // not to sure by bool is an array
                break;
            case CHAR:
                printf("%s%s", record[i].c, extra_space);
                break;
            case VARCHAR:
                printf("%s%s", record[i].v, extra_space);
                break;
        }
    }
}

union record_item create_record_item(int *flag, Attribute attribute, char *value) {
    union record_item recordItem;

    if (attribute->constraints->notnull == true && strcasecmp(value, "null") == 0) {
        fprintf(stderr, "Error: %s cannot be null.\n", attribute->name);
        flag[0] = -1;
        return recordItem;
    }

    int string_size;
    if (attribute->type == CHAR || attribute->type == VARCHAR) {
        string_size = attribute->size;
        // If '"' is on both ends of the string, add 2 to the allowed size of the string
        // since, they don't technically count towards the total size of the string.
        if (value[0] == '"' && value[strlen(value) - 1] == '"') {
            string_size += 2;
        }
    }

    switch (attribute->type) {
        case INTEGER:
            if (strcasecmp(value, "null") == 0) {
                recordItem.i = INT_MIN;
            } else {
                recordItem.i = atoi(value);
            }
            break;
        case DOUBLE:
            if (strcasecmp(value, "null") == 0) {
                recordItem.d = DBL_MIN;
            } else {
                recordItem.d = atof(value);
            }
            break;
        case BOOL:
            if (strcasecmp(value, "null") == 0) {
                recordItem.b[0] = NULL;
                recordItem.b[1] = 0;
            } else if (strcasecmp(value, "true") == 0) {
                recordItem.b[0] = true;
                recordItem.b[1] = 1;
            } else {
                recordItem.b[0] = false;
                recordItem.b[1] = 1;
            }
            break;
        case CHAR:
            if (strcasecmp(value, "null") == 0 || strlen(value) == string_size) {
                strcpy(recordItem.c, value);
            } else { // string size isn't correct
                fprintf(stderr, "Error: %s's length must be equal to %d.\n", value, attribute->size);
                flag[0] = -1;
                return recordItem;
            }
            break;
        case VARCHAR:
            if (strcasecmp(value, "null") == 0 || strlen(value) <= string_size) {
                strcpy(recordItem.v, value);
            } else { // string size isn't correct
                fprintf(stderr, "Error: %s's length must be <= to %d.\n", value, attribute->size);
                flag[0] = -1;
                return recordItem;
            }
            break;
        default:
            break;
    }
    return recordItem;
}

union record_item *create_record_from_statement(Table table, char *tuple) {

    union record_item *record = malloc(sizeof(union record_item) * table->attribute_count);

    char *temp = malloc(strlen(tuple) + 1);
    strcpy(temp, tuple);
    Tuple record_tuple = create_tuple(temp);

    if (record_tuple == NULL) {
        free(temp);
        return NULL;
    }

    for (int i = 0; i < table->attribute_count; i++) {

        int *flag = malloc(sizeof(int) * 1);
        flag[0] = 0;
        union record_item recordItem = create_record_item(flag, table->attributes[i], record_tuple->tuple[i]);

        // check for any potential errors
        if (flag[0] == -1) {
            free(flag);
            free(temp);
            free_tuple(record_tuple);
            freeRecord(record);
            return NULL;
        }

        free(flag);
        record[i] = recordItem;
    }

    free_tuple(record_tuple);
    free(temp);
    return record;
}

int parse_insert_statement(char *statement) {

    char *tokenizer = strtok(statement, " "); // "insert"

    tokenizer = strtok(NULL, " ");
    if (tokenizer != NULL && strcasecmp(tokenizer, "into") == 0) {

        char *table_name = strtok(NULL, " "); // table name
        if (table_name != NULL) {

            // Note: No need to free this table since it will
            // be free'd upon termination of application.
            Table table = get_table_from_catalog(table_name);

            if (table == NULL) { // table doesn't exist
                fprintf(stderr, "Error: Table %s does not exist.\n", table_name);
                return -1;
            }

            tokenizer = strtok(NULL, " ");
            if (tokenizer != NULL && strcasecmp(tokenizer, "values") == 0) {

                tokenizer = strtok(NULL, ";");
                char *tuples = malloc(strlen(tokenizer) + 1);
                strcpy(tuples, tokenizer);

                char *tuple_token;
                char *tuple = strtok_r(tuples, ",", &tuple_token);
                while (tuple != NULL) {

                    union record_item *record = create_record_from_statement(table, tuple);

                    if (record != NULL) {
                        // NOTE: THIS IS FOR TESTING ONLY. Do not include this line for Phase3 submission.
                        print_record(table, record);

                        // insert the tuple into the storage manager
                        if (insert_record(table->tableId, record) == -1) {
                            fprintf(stderr, "Error: Cannot insert:\n\t");
                            print_record(table, record);
                            free(tuples);
                            freeRecord(record);
                            return -1;
                        }

                        freeRecord(record);
                    } else {
                        // is null -> error, don't read any more tuples.
                        free(tuples);
                        freeRecord(record);
                        return -1;
                    }

                    tuple = strtok_r(NULL, ",", &tuple_token);
                }
                free(tuples);
                return 0;
            }
        }
    }
    // bad keyword
    return -1;
}

void free_table_from_storagemanager(int table_size, union record_item **storagemanager_table) {
    if (storagemanager_table != NULL) {
        for (int i = 0; i < table_size; i++) {
            free(storagemanager_table[i]);
        }
        free(storagemanager_table);
    }
}

// TODO
int parse_update_statement(char *statement) {
    char *temp_statement = malloc(strlen(statement) + 1);
    strcpy(temp_statement, statement);

    StringArray statement_array = string_to_array(temp_statement);
    free(temp_statement);

    if (get_index_of_word_from_string(statement, "update") == -1 ||
        get_index_of_word_from_string(statement, "set") == -1) {
        fprintf(stderr, "Error: Invalid command.\n"); // missing 'update' and 'set'. 'Where' is optional.

        free_string_array(statement_array);
        return -1;
    }

    int index = 0;
    if (statement_array->array[index] != NULL && strcasecmp(statement_array->array[index++], "update") == 0) {

        if (statement_array->array[index] != NULL) {
            char *table_name = statement_array->array[index++];
            Table table = get_table_from_catalog(table_name);

            if (table != NULL) {

                char *set_clause = NULL;
                char *where_clause = NULL;
                Clause set = NULL;
                Clause where = NULL;

                // Determine if a where clause exists.
                bool includes_where = false;
                if (get_index_of_word_from_string(statement, "where") >= 0) {
                    includes_where = true;
                }

                // Build the set clause
                if (includes_where) {
                    set_clause = array_of_tokens_to_string(statement_array, "set", "where", false);
                } else {
                    set_clause = array_of_tokens_to_string(statement_array, "set", END_OF_ARRAY, false);
                }

                if (set_clause != NULL) {
                    set = parse_set_clause(set_clause);
                    set->table = table;

                    if (set != NULL) {


                        if (includes_where) {
                            where_clause = array_of_tokens_to_string(statement_array, "where", END_OF_ARRAY, false);
                            where = parse_where_clause(where_clause);
                            where->table = table;
                        }

                        union record_item **storagemanager_table = NULL;

                        int table_size = get_records(table->tableId, &storagemanager_table);
                        if (table_size == -1) {
                            if (includes_where) {
                                free(where_clause);
                                free_clause(where);
                            }
                            free_clause(set);
                            free(set_clause);
                            free_string_array(statement_array);
                            return -1;
                        }

                        if (includes_where) {
                            // grab all records that follow pertain to the clause
                            // iterate through all records then update values based on set clause
//                            for(int i = 0; i < table_size; i++){
//                                bool flags[where->clause_count];
//                                for(int j = 0; j < where->clause_count; j++){
//                                    char ** tmp_clause = string_to_array(where->clauses[i]);
//
//                                    int tmp_clause_index = 0;
//                                    // get records that are in accordance with the clauses.
//                                    while(tmp_clause[tmp_clause_index]){
//                                        // check if record follows where clause
//                                        // if it does, set flag to true
//                                    }
//
//
//                                    free_string_array(tmp_clause);
//                                }
//                                // if flags array is true at each index, leave record,
//                                // otherwise null record in table since we don't need it.
//                            }
                        } else {
                            // todo
                            // iterate through all records then update values based on set clause
                            for (int i = 0; i < table_size; i++) {
                                union record_item *record = storagemanager_table[i]; // todo: may need to free this

                                for (int j = 0; j < set->clauses->size; j++) {
                                    StringArray tmp_clause = string_to_array(set->clauses->array[j]);

                                    char *attribute_name = tmp_clause->array[0];
                                    Attribute attribute = get_attribute_from_table(table, attribute_name);

                                    if (attribute != NULL) {
                                        int record_index = get_attribute_index(table, attribute);
                                        if (record_index == -1) {
                                            free_string_array(tmp_clause);
                                            free_clause(set);
                                            free(set_clause);
                                            free_string_array(statement_array);
                                            free_table_from_storagemanager(table_size, storagemanager_table);
                                            return -1;
                                        }

                                        switch (attribute->type) {
                                            double res;
                                            case INTEGER:
                                                res = calculate_value(set, tmp_clause, record);
                                                record[record_index].i = (int) res;
                                                if (res == DBL_MAX || update_record(table->tableId, record) == -1) {
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
                                                break;
                                            case DOUBLE:
                                                res = calculate_value(set, tmp_clause, record);
                                                record[record_index].d = res;
                                                if (res == DBL_MAX || update_record(table->tableId, record) == -1) {
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
                                                break;
                                            case BOOL:
                                                break;
                                            case CHAR:
                                                break;
                                            case VARCHAR:
                                                break;
                                        }
                                    } else {
                                        // return error
                                        fprintf(stderr, "Error: %s does not exist as an attribute within %s.\n",
                                                attribute_name, table->name);
                                        free_string_array(tmp_clause);
                                        free_clause(set);
                                        free(set_clause);
                                        free_string_array(statement_array);
                                        free_table_from_storagemanager(table_size, storagemanager_table);
                                        return -1;
                                    }
                                    free_string_array(tmp_clause);
                                }
                            }
                        }

                        // updating pseudocode:
                        // for each record
                        // select attribute column
                        // int value = record[column]
                        // if clause contains attribute name operator and value (ex, bar = bar + 1)
                        //   perform necessary operations
                        // else (ex bar = 3)
                        //   update the record with specified value


                        if (includes_where) {
                            free(where_clause);
                            free_clause(where);
                        }
                        free_clause(set);
                        free(set_clause);
                        free_string_array(statement_array);
                        free_table_from_storagemanager(table_size, storagemanager_table);
                        return 0;

                    }
                    free(set_clause);
                }
            }
        }
    }
    free_string_array(statement_array);
    // bad keyword
    return -1;
}

// TODO
int parse_delete_from_statement(char *statement) { return 0; }

// TODO
int parse_select_statement(char *statement) { return 0; }