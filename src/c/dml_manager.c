#include "../headers/dml_manager.h"
#include "../headers/Enums.h"
#include "../headers/catalog.h"
#include "../headers/tuple.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char *record_item_to_string(Type type, union record_item item) {
    char *ret = NULL;
    size_t size;
    switch (type->type_num) {
        case INTEGER:
            size = sizeof(item.i) + 1;
            ret = malloc(sizeof(char *) * size);
            snprintf(ret, (item.i + 1), "%d", item.i);
            return ret;
        case DOUBLE:
            size = sizeof(item.d) + 1;
            ret = malloc(sizeof(char *) * size);
            snprintf(ret, size, "%lf", item.d);
            return ret;
        case BOOL:
            return item.b[0] == true ? "true" : (item.b[0] == false ? "false" : "null");
        case CHAR:
            ret = malloc(strlen(item.c) + 1);
            strcpy(ret, item.c);
            return ret;
        case VARCHAR:
            ret = malloc(strlen(item.v) + 1);
            strcpy(ret, item.v);
            return ret;
    }
    return ret;
}

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
    printf("|");
    for (int i = 0; i < table->attrs_size; i++) {
        char *extra_space = " ";

        if (i == table->attrs_size - 1) {
            extra_space = ")\n"; // close the tuple, create new line
        }
        switch (table->attrs[i]->type->type_num) {
            case INTEGER:
                printf(" %10d |", record[i].i);
                break;
            case DOUBLE:
                printf(" %10f |", record[i].d);
                break;
            case BOOL:
                printf(" %10.10s |", (record[i].b[0] == 1) ? "true" : "false"); // not to sure by bool is an array
                break;
            case CHAR:
                printf(" %10.10s |", record[i].c);
                break;
            case VARCHAR:
                printf(" %10.10s |", record[i].v);
                break;
        }
    }
}
void print_record_as_tuple(Table table, union record_item *record) {
    printf("(");
    for (int i = 0; i < table->attrs_size; i++) {
        char *extra_space = " ";

        if (i == table->attrs_size - 1) {
            extra_space = ")\n"; // close the tuple, create new line
        }
        switch (table->attrs[i]->type->type_num) {
            case INTEGER:
                printf("%d%s", record[i].i, extra_space);
                break;
            case DOUBLE:
                printf("%f%s", record[i].d, extra_space);
                break;
            case BOOL:
                printf("%s%s", (record[i].b[0] == 1) ? "true" : "false",
                       extra_space); // not to sure by bool is an array
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

union record_item create_record_item(int *flag, Attr attribute, char *value) {
    union record_item recordItem;

    int data_type = get_data_type(value);
    if (data_type != attribute->type->type_num && data_type) {
        // don't error if data_type is returned as a char and the attribute is a varchar.
        // also don't error if either the data type or the attribute type is an integer or double.
        if (!(data_type == CHAR && attribute->type->type_num == VARCHAR) &&
            !((data_type == INTEGER && attribute->type->type_num == DOUBLE)
              || (data_type == DOUBLE && attribute->type->type_num == INTEGER))) {
            fprintf(stderr, "Error: cannot insert unmatched data types.\n");
            flag[0] = -1;
            return recordItem;
        }
    }

    if (attribute->notnull == true && strcasecmp(value, "null") == 0) {
        fprintf(stderr, "Error: %s cannot be null.\n", attribute->name);
        flag[0] = -1;
        return recordItem;
    }

    int string_size;
    if (attribute->type->type_num == CHAR || attribute->type->type_num == VARCHAR) {
        string_size = (int) attribute->type->num_chars;
        // If '"' is on both ends of the string, add 2 to the allowed size of the string
        // since, they don't technically count towards the total size of the string.
        if (value[0] == '"' && value[strlen(value) - 1] == '"') {
            string_size += 2;
        } else if (value[0] != '"' && value[strlen(value) - 1] != '"') {
            fprintf(stderr, "Error: string values must be wrapped in quotes.\n");
            flag[0] = -1;
            return recordItem;
        }
    }

    switch (attribute->type->type_num) {
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
                fprintf(stderr, "Error: %s's length must be equal to %d.\n", value, attribute->type->num_chars);
                flag[0] = -1;
                return recordItem;
            }
            break;
        case VARCHAR:
            if (strcasecmp(value, "null") == 0 || strlen(value) <= string_size) {
                strcpy(recordItem.v, value);
            } else { // string size isn't correct
                fprintf(stderr, "Error: %s's length must be <= to %d.\n", value, attribute->type->num_chars);
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

    union record_item *record = malloc(sizeof(union record_item) * table->attrs_size);

    char *temp = malloc(strlen(tuple) + 1);
    strcpy(temp, tuple);
    Tuple record_tuple = create_tuple(temp);

    if (record_tuple == NULL) {
        free(temp);
        return NULL;
    }

    for (int i = 0; i < table->attrs_size; i++) {

        int *flag = malloc(sizeof(int) * 1);
        flag[0] = 0;
        union record_item recordItem = create_record_item(flag, table->attrs[i], record_tuple->tuple[i]);

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
            Table table = get_table_by_name(table_name);

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

                        // insert the tuple into the storage manager
                        if (insert_record(table->num, record) == -1) {
                            fprintf(stderr, "Error: Cannot insert:\t");
                            print_record_as_tuple(table, record);
                            free(tuples);
                            freeRecord(record);
                            return -1;
                        }
                        print_record_as_tuple(table, record);
                        printf("has been inserted.\n\n");
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
            Table table = get_table_by_name(table_name);

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

                        int table_size = get_records(table->num, &storagemanager_table);

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

                        // iterate through all records then update values based on set clause
                        for (int i = 0; i < table_size; i++) {
                            union record_item *record = storagemanager_table[i];
                            bool can_update = true;

                            if (includes_where) {
                                can_update = record_satisfies_where(where, record);
                            }

                            if (can_update) {
                                print_record_as_tuple(table, record);

                                // Modify the record based on the set clause.
                                for (int j = 0; j < set->clauses->size; j++) {
                                    StringArray tmp_clause = string_to_array(set->clauses->array[j]);
                                    char *attribute_name = tmp_clause->array[0];
                                    Attr attribute = get_attr_by_name(table, attribute_name);
                                    Attr attribute2 = NULL;
                                    int string_size;
                                    int string_size_attribute2;

                                    if (attribute != 0) {
                                        int record_index = attribute->position;
                                        if (record_index == -1) {
                                            free_string_array(tmp_clause);
                                            free_clause(set);
                                            free(set_clause);
                                            free_string_array(statement_array);
                                            free_table_from_storagemanager(table_size, storagemanager_table);
                                            return -1;
                                        }

                                        if (strcasecmp(tmp_clause->array[2], "null") == 0 && attribute->notnull) {
                                            fprintf(stderr,
                                                    "Error: Cannot insert null into attribute %s since notnull was specified.\n",
                                                    attribute_name);
                                            free_string_array(tmp_clause);
                                            free_clause(set);
                                            free(set_clause);
                                            free_string_array(statement_array);
                                            free_table_from_storagemanager(table_size, storagemanager_table);
                                            return -1;
                                        }

                                        if (attribute->type->type_num == CHAR || attribute->type->type_num == VARCHAR) {
                                            string_size = (int) attribute->type->num_chars;
                                            // If '"' is on both ends of the string, add 2 to the allowed size of the string
                                            // since, they don't technically count towards the total size of the string.
                                            char *value = tmp_clause->array[2];
                                            if (value[0] == '"' && value[strlen(value) - 1] == '"') {
                                                string_size += 2;
                                            }
                                        }
                                        // todo: check it primary key can be modified.
                                        switch (attribute->type->type_num) {
                                            double res;
                                            case INTEGER:
                                                res = calculate_value(set, tmp_clause, record);
                                                record[record_index].i = (int) res;
                                                if (res == DBL_MAX) {
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
                                                if (res == DBL_MAX) {
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
                                                break;
                                            case BOOL:
                                                // handle inserting null (includes all records).

                                                // check to see if another column value
                                                attribute2 = get_attr_by_name(table, tmp_clause->array[2]);
                                                if (attribute2 != 0 && attribute2->type->type_num == BOOL) {
                                                    record[record_index].b[0] = record[attribute2->position].b[0];
                                                } else if (strcasecmp(tmp_clause->array[2], "true") == 0
                                                           || strcasecmp(tmp_clause->array[2], "false") == 0
                                                           || strcasecmp(tmp_clause->array[2], "null") == 0) {

                                                    if (strcasecmp(tmp_clause->array[2], "true") == 0) {
                                                        record[record_index].b[0] = true;
                                                    }
                                                    if (strcasecmp(tmp_clause->array[2], "false") == 0) {
                                                        record[record_index].b[0] = NULL;
                                                    } else {
                                                        record[record_index].b[0] = false;
                                                    }
                                                } else {
                                                    fprintf(stderr, "Error: Invalid value of %s passed in for attribute %s.\n",
                                                            tmp_clause->array[2], attribute_name);
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
                                                break;
                                            case CHAR:
                                                // check to see if another column value
                                                attribute2 = get_attr_by_name(table, tmp_clause->array[2]);
                                                if (attribute2 != 0 && attribute2->type->type_num == CHAR) {

                                                    string_size_attribute2 = attribute2->type->num_chars;

                                                    // If '"' is on both ends of the string, add 2 to the allowed size of the string
                                                    // since, they don't technically count towards the total size of the string.
                                                    char *value2 = record[attribute2->position].c;
                                                    if (value2[0] == '"' && value2[strlen(value2) - 1] == '"') {
                                                        string_size_attribute2 += 2;
                                                    }

                                                    // determine if attribute 2 meets the size constraints for attribute 1
                                                    if (strlen(record[attribute2->position].c) !=
                                                        string_size_attribute2) {
                                                        fprintf(stderr, "Error: %s's length must be equal to %d.\n",
                                                                attribute2->name, attribute->type->num_chars);
                                                        free_string_array(tmp_clause);
                                                        free_clause(set);
                                                        free(set_clause);
                                                        free_string_array(statement_array);
                                                        free_table_from_storagemanager(table_size,
                                                                                       storagemanager_table);
                                                        return -1;
                                                    }

                                                    strcpy(record[record_index].c, record[attribute2->position].c);
                                                } else if (strcasecmp(tmp_clause->array[2], "null") == 0 ||
                                                           strlen(tmp_clause->array[2]) == string_size) {
                                                    strcpy(record[record_index].c, tmp_clause->array[2]);
                                                } else { // error
                                                    fprintf(stderr, "Error: %s's length must be equal to %d.\n",
                                                            tmp_clause->array[2], attribute->type->num_chars);
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
                                                break;
                                            case VARCHAR:
                                                // check to see if another column value
                                                attribute2 = get_attr_by_name(table, tmp_clause->array[2]);

                                                if (attribute2 != 0 && attribute2->type->type_num == VARCHAR) {

                                                    string_size_attribute2 = attribute2->type->num_chars;

                                                    // If '"' is on both ends of the string, add 2 to the allowed size of the string
                                                    // since, they don't technically count towards the total size of the string.
                                                    char *value2 = record[attribute2->position].c;
                                                    if (value2[0] == '"' && value2[strlen(value2) - 1] == '"') {
                                                        string_size_attribute2 += 2;
                                                    }

                                                    // determine if attribute 2 meets the size constraints for attribute 1
                                                    if (strlen(record[attribute2->position].c) >
                                                        string_size_attribute2) {
                                                        fprintf(stderr, "Error: %s's length must be equal to %d.\n",
                                                                attribute2->name, attribute->type->num_chars);
                                                        free_string_array(tmp_clause);
                                                        free_clause(set);
                                                        free(set_clause);
                                                        free_string_array(statement_array);
                                                        free_table_from_storagemanager(table_size,
                                                                                       storagemanager_table);
                                                        return -1;
                                                    }

                                                    strcpy(record[record_index].c, record[attribute2->position].c);
                                                } else if (strcasecmp(tmp_clause->array[2], "null") == 0 ||
                                                           strlen(tmp_clause->array[2]) <= string_size) {
                                                    strcpy(record[record_index].v, tmp_clause->array[2]);
                                                } else { // string size isn't correct
                                                    fprintf(stderr,
                                                            "Error: %s's length must be less than or equal to %d.\n",
                                                            tmp_clause->array[2], attribute->type->num_chars);
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
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

                                if (update_record(table->num, record) == -1) {
                                    fprintf(stderr, "An error has occurred in an attempt to update a record.\n");
                                    free_clause(set);
                                    free(set_clause);
                                    free_string_array(statement_array);
                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                    return -1;
                                }
                                printf("has been updated to: ");
                                print_record_as_tuple(table, record);
                            }
                        }

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
int parse_delete_from_statement(char *statement) {

    // delete token
    char *token = strtok(statement, " ");
    //printf("delete token: %s\n", token);

    // expected to be "from"
    token = strtok(NULL, " ");
    //printf("from token: %s\n", token);

    if (token == NULL && strcasecmp(token, "from") != 0) {
        fprintf(stderr, "Error: expected \"from\" got %s\n", token);
        return -1;
    }

    // expected to be existing table name
    char *table_name = strtok(NULL, " ");
    //printf("table_name: %s\n", table_name);
    Table table = get_table_by_name(table_name);

    if (table == NULL) { // table doesn't exist
        fprintf(stderr, "Error: Table %s does not exist.\n", table_name);
        return -1;
    }

    // expected to be "where"
    token = strtok(NULL, " ");
    //printf("where token: %s\n", token);

    if (token == NULL && strcasecmp(token, "where") != 0) {
        fprintf(stderr, "Error: expected \"where\" got %s\n", token);
        return -1;
    }

    union record_item **records = NULL;
    int table_size = get_records(table->num, &records);

    if (table_size == -1) {
        fprintf(stderr, "Error: unable to records from table %s\n", table_name);
        return -1;
    }

    // conditionals
    char *condition = strtok(NULL, ";");

    if (DEBUG == 1) {
        printf("condition: %s\n", condition);
    }

    // parse where clause
    Clause where_clause = parse_where_clause(condition);
    where_clause->table = table;

    union record_item *primary_key;
    union record_item *current_record;
    int primary_key_size;
    int position;
    Unique primary;
    int remove_result = 0;

    for (int i = 0; i < table_size; i++) {
        current_record = records[i];

        if (record_satisfies_where(where_clause, current_record)) {
            primary = get_primary_key(where_clause->table);
            primary_key_size = get_unique_attrs_size(primary);
            primary_key = malloc(sizeof(union record_item) * primary_key_size);

            for (int j = 0; j < primary_key_size; j++) {
                position = get_attr_position(primary->attrs[j]);
                primary_key[j] = current_record[position];
            }
            remove_result = remove_record(table->num, primary_key);
        }
    }

    records = NULL;
    table_size = get_records(table->num, &records);

    if (DEBUG == 1) {
        printf("\n after delete:\n");
        for (int i = 0; i < table_size; i++) {
            print_record_as_tuple(table, records[i]);
        }
    }

    free_clause(where_clause);
    free_table_from_storagemanager(table_size, records);
    return remove_result;
}

int parse_select_statement(char *statement, union record_item ***result) {
    if (DEBUG == 1) {
        printf("Going!\n");
    }
    char *temp_statement = malloc(strlen(statement) + 1);
    strcpy(temp_statement, statement);

    StringArray statement_array = string_to_array(temp_statement);
    free(temp_statement);

    if (get_index_of_word_from_string(statement, "select") == -1 ||
        get_index_of_word_from_string(statement, "from") == -1) {
        fprintf(stderr, "Error: Invalid command.\n"); // missing 'select' and 'from'. 'where' is optional.

        free_string_array(statement_array);
        return -1;
    }

    int index = 0;
    if (statement_array->array[index] != NULL && strcasecmp(statement_array->array[index++], "select") == 0) {

        //check for where clause, will determine how we find the from clause
        bool includes_where = false;
        if (get_index_of_word_from_string(statement, "where") >= 0) {
            includes_where = true;
        }
        char *from_clause = NULL;
        if (includes_where) {
            from_clause = array_of_tokens_to_string(statement_array, "from", "where", false);
        } else {
            from_clause = array_of_tokens_to_string(statement_array, "from", END_OF_ARRAY, false);
        }

        // get multiple table names
        char *from_token;
        char **table_names = calloc(3, sizeof(char *));
        char *table_name = strtok_r(from_clause, ", ", &from_token);
        table_names[0] = table_name;
        int names_length = 3; // start off with a little bit of space
        int name_index = 1;

        while ((table_name = strtok_r(NULL, ", ", &from_token)) != NULL) {
            if (DEBUG == 1) {
                printf("table name: %s\n", table_name);
            }
            if (name_index >= names_length) {
                table_names = realloc(table_names, names_length * 2 *
                                                   sizeof(char *)); // double the length every time to reduce calls to realloc
                names_length *= 2;
            }
            table_names[name_index] = table_name;
            name_index++;
            if (DEBUG == 1) {
                printf("name_index: %d\n", name_index);
            }
        }

        if (DEBUG == 1) {
            printf("table names: ");
            for (int i = 0; i < name_index; i++) {
                printf("%s ", table_names[i]);
            }
            printf("\n");
        }

        Table *table_list = malloc(name_index * sizeof(Table));
        for (int i = 0; i < name_index; i++) {
            Table table = get_table_by_name(table_names[i]);
            if (table != NULL) {
                table_list[i] = table;
            } else {
                fprintf(stderr, "Error: could not find table %s\n", table_names[i]);
                return -1;
            }
        }

        char *select_clause = NULL;
        char *where_clause = NULL;

        // Build the select clause
        // this c
        select_clause = array_of_tokens_to_string(statement_array, "select", "from", false);

        // TODO: determine selected columns
        // check for correct syntax
        char *column_token;
        // Assume that we'll have at least one column from every table
        char **columns = calloc(names_length, sizeof(char *));
        char *column_name = strtok_r(select_clause, ", ", &column_token);
        columns[0] = column_name;
        int column_index = 1;
        int columns_length = names_length;

        bool found_star;
        if (strcmp(column_name, "*") == 0) {
            found_star = true;
        } else {
            found_star = false;
        }

        while ((column_name = strtok_r(NULL, ", ", &column_token)) != NULL) {
            if (found_star) {
                fprintf(stderr, "Error: cannot specify additional columns alongside a *\n");
                return -1;
            }
            if (column_index >= columns_length) {
                columns = realloc(columns, columns_length * 2 * sizeof(char *));
                columns_length *= 2;
            }
            columns[column_index] = column_name;
            if (strcmp(column_name, "*") == 0) {
                fprintf(stderr, "Error: cannot specify additional columns alongside a *\n");
                return -1;
            }
            column_index++;
        }

        if (DEBUG == 1) {
            printf("column names: ");
            for (int j = 0; j < column_index; j++) {
                printf("%s ", columns[j]);
            }
            printf("\n");
        }

        for (int j = 0; j < column_index; j++) {
            if (DEBUG == 1) {
                printf("looking for column %s\n", columns[j]);
            }
            if (strstr(columns[j], ".")) {
                // table.column syntax
                // check through the tables to see if one matches the first part
                for (int k = 0; k < name_index; k++) {
                    if (strncasecmp(columns[j], table_list[k]->name, strstr(columns[j], ".") - columns[j] - 1) == 0) {
                        // got a table that matches
                        int found_col = 0;
                        for (int l = 0; l < table_list[k]->attrs_size; l++) { // Four loops deep!
                            if (strcasecmp(strstr(columns[j], ".") + 1, table_list[k]->attrs[l]->name) == 0) {
                                // Got it!
                                found_col = 1;
                                break;
                            }
                        }
                        if (!found_col) {
                            fprintf(stderr, "Error: could not find column %s\n", columns[j]);
                            return -1;
                        }
                    }
                }
            } else {
                // could be a unique name or could be an error
                int found_col = 0;
                for (int k = 0; k < name_index; k++) {
                    for (int l = 0; l < table_list[k]->attrs_size; l++) {
                        if (strcasecmp(columns[j], table_list[k]->attrs[l]->name) == 0) {
                            // Found one
                            found_col++;
                            if (found_col > 1) {
                                // Found the name twice, so they need to specify which one belongs to which table
                                fprintf(stderr,
                                        "Error: attributes in different tables with the same name must be individually identified\n");
                                return -1;
                            }
                        }
                    }
                }
                if (found_col != 1 && (strcmp(columns[j], "*") != 0)) {
                    fprintf(stderr, "Error: could not find column %s\n", columns[j]);
                    return -1;
                }
            }
        }

        if (DEBUG == 1) {
            printf("done checking columns\n");
        }

        union record_item ***record_lists = calloc(name_index, sizeof(union record_item *));

        int *attrs_per_table = malloc(name_index * sizeof(int));
        int table_attrs_index = 0;

        int *records_per_table = malloc(name_index * sizeof(int));
        int table_records_index = 0;

        // Got to be very careful about freeing this. If found_star = false, must free each attribute and it's name
        Table result_table = create_table(-1, "Result");

        // All of this will need to support multiple tables
        for (int i = 0; i < name_index; i++) {
            // These will all be non-null at this point
            Table table = table_list[i];

            union record_item **storagemanager_table;

            int table_size = get_records(table->num, &storagemanager_table);

            if (found_star) {
                record_lists[i] = storagemanager_table;
                for (int l = 0; l < table->attrs_size; l++) {
                    add_attr(result_table, table->attrs[l]);
                }
                records_per_table[table_records_index] = table_size;

                // Store how many attributes total this table had
                attrs_per_table[table_attrs_index] = table->attrs_size;
                table_attrs_index++;

                // Store how many tuples this table had
                records_per_table[table_records_index] = table_size;
                table_records_index++;

                union record_item **filtered_records = malloc(
                        table_size * table->attrs_size * sizeof(union record_item));

                for (int j = 0; j < table_size; j++) {
                    union record_item *filtered_tuple = malloc(sizeof(union record_item) * table->attrs_size);
                    for (int k = 0; k < table->attrs_size; k++) {
                        union record_item *copy = malloc(sizeof(union record_item));
                        memcpy(copy, &(storagemanager_table[j][k]), sizeof(union record_item));
                        filtered_tuple[k] = *copy;
                    }
                    filtered_records[j] = filtered_tuple;
                }
                record_lists[i] = filtered_records;
                // If we're not using the entire record set as returned, we've already copied the data, so free this
                free(storagemanager_table);
            } else {
                //array that would hold the indexes of attr from this table
                int *attr_indexes = calloc(table->attrs_size, sizeof(int)); // Might be less, won't be more
                int num_cols_in_table = 0;
                for (int col = 0; col < column_index; col++) {
                    Attr result_attr = NULL;
                    if (strstr(columns[col], ".")) {
                        if (strncasecmp(columns[col], table->name, strstr(columns[col], ".") - columns[col]) == 0) {
                            for (int l = 0; l < table->attrs_size; l++) {
                                if (strcasecmp(strstr(columns[col], ".") + 1, table->attrs[l]->name) == 0) {
                                    attr_indexes[num_cols_in_table] = l;
                                    result_attr = create_attr(columns[col], num_cols_in_table, table->attrs[l]->type,
                                                              table->attrs[l]->notnull);
                                    add_attr(result_table, result_attr);
                                    num_cols_in_table++;
                                }
                            }
                        }

                    } else {
                        for (int l = 0; l < table->attrs_size; l++) {
                            if (strcasecmp(columns[col], table->attrs[l]->name) == 0) {
                                attr_indexes[num_cols_in_table] = l;
                                result_attr = create_attr(columns[col], num_cols_in_table, table->attrs[l]->type,
                                                          table->attrs[l]->notnull);
                                add_attr(result_table, result_attr);
                                num_cols_in_table++;
                            }
                        }
                    }
                }

                // Store how many attributes total this table had
                attrs_per_table[table_attrs_index] = num_cols_in_table;
                table_attrs_index++;

                // Store how many tuples this table had
                records_per_table[table_records_index] = table_size;
                table_records_index++;

                union record_item **filtered_records = malloc(
                        table_size * num_cols_in_table * sizeof(union record_item));

                for (int j = 0; j < table_size; j++) {
                    union record_item *filtered_tuple = malloc(sizeof(union record_item) * num_cols_in_table);
                    for (int k = 0; k < num_cols_in_table; k++) {
                        union record_item *copy = malloc(sizeof(union record_item));
                        memcpy(copy, &(storagemanager_table[j][attr_indexes[k]]), sizeof(union record_item));
                        filtered_tuple[k] = *copy;
                    }
                    filtered_records[j] = filtered_tuple;
                }
                record_lists[i] = filtered_records;
                // If we're not using the entire record set as returned, we've already copied the data, so free this
                free(storagemanager_table);
            }
        }

        /*
        printf("got: ");
        for (int i = 0; i < 3; i++) {
            printf("%d ", record_lists[0][i][0].i);
        }
        printf("\n");
        */

        // Time for cartesian product if we have more than one table
        // variable for current cartesian product (starts as just the first set of records)
        union record_item **product = record_lists[0];
        // variable to store the result of the cartesian product (copy into first var after calculation)
        union record_item **result_product;
        // variable for size of the starting cartesian product (starts at size of first record set)
        int product_size = records_per_table[0];

        // keep track of how many attrs are in the tuple so far
        if (DEBUG == 1) {
            for (int i = 0; i < table_attrs_index; i++) {
                printf("%d ", attrs_per_table[i]);
            }
            printf("\n");
        }
        int total_attrs = attrs_per_table[0];

        // shared variable somewhere for index of result array
        int result_index;

        if (name_index > 1) {
            for (int i = 1; i < name_index; i++) {
                // Cartesian product the first set with all subsequent sets in order (no optimization)

                // set result index to 0
                result_index = 0;

                // find out how many attrs were requested from next table add to amount of attrs total
                int new_total_attrs = total_attrs + attrs_per_table[i];

                // malloc the result array to be product_size * table's records * new total attrs
                result_product = malloc(
                        product_size * records_per_table[i] * new_total_attrs * sizeof(union record_item));

                // Loop through all records in current cartesian product result
                for (int j = 0; j < product_size; j++) {

                    // Loop through every attribute in the new record
                    for (int k = 0; k < records_per_table[i]; k++) {
                        // malloc a new tuple that can store all the old records plus the new ones
                        union record_item *new_tuple = malloc(new_total_attrs * sizeof(union record_item));

                        if (DEBUG == 1) {
                            printf("new tuple number %d\n", result_index);
                        }

                        // memcpy the records from loop j into the first part, memcpy the records from loop k into the second
                        for (int l = 0; l < total_attrs; l++) {
                            if (DEBUG == 1) {
                                printf("putting %d at %d\n", product[j][l].i, l);
                            }
                            memcpy(&new_tuple[l], &product[j][l], sizeof(union record_item));
                        }

                        for (int l = 0; l < attrs_per_table[i]; l++) {
                            if (DEBUG == 1) {
                                printf("putting %d at %d\n", record_lists[i][k][l].i, l + total_attrs);
                            }
                            memcpy(&new_tuple[l + total_attrs], &record_lists[i][k][l], sizeof(union record_item));
                        }

                        result_product[result_index] = new_tuple;
                        if (DEBUG == 1) {
                            printf("contents of new tuple: ");
                            for (int bleh = 0; bleh < new_total_attrs; bleh++) {
                                printf("%d ", new_tuple[bleh].i);
                            }
                            printf("\n");
                        }

                        result_index++;
                        // Put that new record at the next spot in the result array
                        // increment the index
                    }
                }
                // free the product array
                free(product);
                // set product to point to the result array
                product = result_product;
                // null out the result pointer
                result_product = NULL;
                if (DEBUG == 1) {
                    printf("total_attrs was %d and is now %d\n", total_attrs, new_total_attrs);
                }
                total_attrs = new_total_attrs;
                product_size *= records_per_table[i];
            }
        }
        if (DEBUG == 1) {
            printf("product size: %d\n", product_size);
            printf("total attrs: %d\n", total_attrs);

            // for manually debugging, remove
            for (int i = 0; i < product_size; i++) {
                for (int j = 0; j < total_attrs; j++) {
                    printf("%d ", product[i][j].i);
                }
                printf("\n");
            }
        }

        // Print the result header
        printf("|");
        for (int l = 0; l < result_table->attrs_size; l++) {
            printf(" %10.10s |", result_table->attrs[l]->name);
        }
        printf("\n|");
        for (int l = 0; l < result_table->attrs_size; l++) {
            printf("____________|");
        }
        printf("\n");

        Clause where = NULL;

        union record_item **filtered_result = malloc(product_size * total_attrs * sizeof(union record_item));
        int filtered_index = 0;

        if (includes_where) {
            where_clause = array_of_tokens_to_string(statement_array, "where", END_OF_ARRAY, false);
            where = parse_where_clause(where_clause);
            where->table = result_table;

            for (int i = 0; i < product_size; i++) {

                union record_item *new_tuple = malloc(total_attrs * sizeof(union record_item));

                if (record_satisfies_where(where, product[i])) {
                    print_record(result_table, product[i]);
                    printf("\n");

                    for (int j = 0; j < total_attrs; j++) {
                        memcpy(&new_tuple[j], &product[i][j], sizeof(union record_item));
                    }
                    filtered_result[filtered_index] = new_tuple;
                    filtered_index++;
                }
            }
            result = &filtered_result;
            free(where_clause);
            free(where);
        } else { // If there is no where clause, print all the records
            for (int record_idx = 0; record_idx < product_size; record_idx++) {
                print_record(result_table, product[record_idx]);
                printf("\n");
            }
            result = &product;
        }

        // Print that big product array somehow
        // free the product array
        // free the result table. Very much depends on whether rows were requested or not.
        free(result_table->name);

        if (!found_star) {
            for (int l = 0; l < result_table->attrs_size; l++) {
                free(result_table->attrs[l]->name);
                free(result_table->attrs[l]);
            }
        }
        free(result_table->attrs);
        free(result_table);

        free(columns);
        free(from_clause);
        free(select_clause);
        if (DEBUG == 1) {
            printf("done\n");
        }
        fflush(stdout);
        result = &result_product;
        return 0;
    }
    free_string_array(statement_array);
    // bad keyword
    return -1;
}

bool record_satisfies_where(Clause where_clause, union record_item *record) {
    StringArray conditions = where_clause->clauses;
    StringArray operators = where_clause->operators;
    char *condition_results = malloc(conditions->size * sizeof(char));
    if (DEBUG == 1) {
        printf("\n");
        print_record_as_tuple(where_clause->table, record);
    }

    char *boolean_string = malloc(sizeof(char) * (conditions->size + operators->size) + 1);
    boolean_string[conditions->size + operators->size] = 0;

    if (conditions->size < 1) {
        fprintf(stderr, "Error: there is zero conditions\n");
        return 0;
    } else if (conditions->size == 1) {
        return does_record_satisfy_condition(record, conditions->array[0], where_clause->table);
    }

    for (int i = 0; i < conditions->size * 2; i += 2) {
        boolean_string[i] = does_record_satisfy_condition(record, conditions->array[i / 2], where_clause->table) + '0';
    }

    for (int i = 1; i < operators->size * 2; i += 2) {
        // using the condition results array preform the boolean logic to get the result being the last index in array
        if (strcasecmp(operators->array[(i - 1) / 2], "or") == 0) {
            boolean_string[i] = '+';
        } else if (strcasecmp(operators->array[(i - 1) / 2], "and") == 0) {
            boolean_string[i] = '*';
        } else {
            fprintf(stderr, "Error: Invalid operator %s\n", operators->array[i]);
            return -1;
        }
    }

    if (DEBUG == 1) {
        printf("boolean_String: %s\n", boolean_string);
    }

    StringArray boolean_expression = expression_to_string_list(boolean_string);

    OperationTree boolean_tree = build_tree(boolean_expression);
    bool result = (bool) evaluate_boolean_tree(boolean_tree->root);

    if (DEBUG == 1) {
        printf("logical_result: %d\n", result);
    }

    freeOperationTree(boolean_tree);
    free(condition_results);
    free(boolean_string);
    return result;
}

int get_records_where_clause(Clause where_clause, union record_item **selected_records) {
    union record_item **records = NULL;
    int record_count = 0;
    bool result;

    // get num the tables id?
    int table_size = get_records(where_clause->table->num, &records);

    if (DEBUG == 1) {
        printf("records_size: %d\n", table_size);
    }

    if (table_size == -1) {
        fprintf(stderr, "Error: unable to get records from table \n");
        return -1;
    }

    // store the boolean result of each condition
    union record_item *record;

    for (int i = 0; i < table_size; i++) {
        record = records[i];

        // set the result of checking if the record passes the condition into the condition results array
        if (record_satisfies_where(where_clause, record)) {
            print_record_as_tuple(where_clause->table, record);
            record_count++;
            if (record_count == 1) {
                selected_records = malloc(sizeof(union record_item *) * record_count);
            } else {
                selected_records = realloc(selected_records, sizeof(union record_item *) * record_count);
            }
            selected_records[record_count - 1] = record;
        }

    }

    if (DEBUG == 1) {
        printf("\n");
    }
    print_record_as_tuple(where_clause->table, selected_records[0]);
    print_record_as_tuple(where_clause->table, selected_records[1]);
    print_record_as_tuple(where_clause->table, selected_records[2]);
    return record_count;
}

StringArray condition_to_expression(union record_item *record, char *condition, Table table) {
    char *temp = strdup(condition);
    char * attribute_name = strtok(temp,"=+/-* ");
    int expression_array_index = 0;
    StringArray expression = expression_to_string_list(condition);

    while(attribute_name != NULL){
        Attr attribute = get_attr_by_name(table, attribute_name);
        if(attribute != NULL){
            char* string_record_item = record_item_to_string(get_attr_type(attribute), record[get_attr_position(attribute)]);
            if (get_attr_type(attribute)->type_num == CHAR || get_attr_type(attribute)->type_num == VARCHAR) {
                remove_spaces(string_record_item);
            }
            expression->array[expression_array_index] = string_record_item;
            expression_array_index += 2;
        }
        attribute_name = strtok(NULL,"=+/-* ");
    }

    free(temp);
    return expression;
}

bool does_record_satisfy_condition(union record_item *record, char *condition, Table table) {
    StringArray expression = condition_to_expression(record, condition, table);

    if (DEBUG == 1) {
        printf("expression: { ");
        for (int i = 0; i < expression->size; i++) {
            if (i == expression->size - 1) {
                printf("%s ", expression->array[i]);
            } else {
                printf("%s,", expression->array[i]);
            }
        }
        printf("}\n");
    }

    OperationTree tree = build_tree(expression);
    bool condition_satisfied = determine_conditional(tree->root);

    if (DEBUG == 1) {
        printf("determine_conditional_result: %d\n", condition_satisfied);
    }

    freeOperationTree(tree);
    return condition_satisfied;
}
