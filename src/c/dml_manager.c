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
            ret = malloc(sizeof(char *)* size);
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
                        print_record(table, record);

                        // insert the tuple into the storage manager
                        if (insert_record(table->num, record) == -1) {
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
                                print_record(table, record);

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
                                                    fprintf(stderr, "Error: Invalid value for %s.\n",
                                                            tmp_clause->array[2]);
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
    printf("condition: %s\n", condition);

    // parse where clause
    Clause where_clause = parse_where_clause(condition);
    where_clause->table = table;

    union record_item *primary_key;
    union record_item *current_record;
    int primary_key_size;
    int position;
    Unique primary;
    int remove_result = 0;

    for(int i = 0; i < table_size; i++) {
        current_record = records[i];

        if(record_satisfies_where(where_clause, current_record)) {
            primary = get_primary_key(where_clause->table);
            primary_key_size = get_unique_attrs_size(primary);
            primary_key = malloc(sizeof(union record_item) * primary_key_size);

            for(int j = 0; j < primary_key_size; j++) {
                position = get_attr_position(primary->attrs[j]);
                primary_key[j] = current_record[position];
            }
            remove_result = remove_record(table->num, primary_key);
        }
    }

    records = NULL;
    table_size = get_records(table->num, &records);

    printf("\n after delete:\n");
    for(int i = 0; i < table_size; i++) {
        print_record(table, records[i]);
    }
    return remove_result;
}

int parse_select_statement(char *statement) {
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
    if (statement_array->array[index] != NULL && strcasecmp(statement_array->array[index++], "update") == 0) {

        int from_clause_index = get_index_of_word_from_string(statement, "from");
        from_clause_index++; // TODO: This is pretty basic atm. must be able to handle multiple tables though
        if (statement_array->array[from_clause_index] != NULL) {
            char *table_name = statement_array->array[from_clause_index];
            Table table = get_table_by_name(table_name);

            if (table != NULL) {

                // TODO: Get the whole of the records from the table if it isn't null.

                char *select_clause = NULL;
                char *where_clause = NULL;
                Clause select = NULL;
                Clause where = NULL;

                // Determine if a where clause exists.
                bool includes_where = false;
                if (get_index_of_word_from_string(statement, "where") >= 0) {
                    includes_where = true;
                }

                // Build the select clause
                select_clause = array_of_tokens_to_string(statement_array, "select", "from", false);

                // TODO: determine selected rows

                if (includes_where) {
                    //TODO: where clause logic
                }
            }
        }
    }
    free_string_array(statement_array);
    // bad keyword
    return -1;
}

bool record_satisfies_where(Clause where_clause, union record_item *record) {
    StringArray conditions = where_clause->clauses;
    StringArray operators = where_clause->operators;
    char *condition_results = malloc(conditions->size * sizeof(char));
    printf("\n");
    print_record(where_clause->table, record);

    char *boolean_string = malloc(sizeof(char) * (conditions->size + operators->size) + 1);
    boolean_string[conditions->size + operators->size] = 0;

    for (int i = 0; i < conditions->size * 2; i+=2) {
        boolean_string[i] = does_record_satisfy_condition(record, conditions->array[i/2], where_clause->table) +'0';
    }

    for (int i = 1; i < operators->size * 2; i+=2) {
        // using the condition results array preform the boolean logic to get the result being the last index in array
        if (strcasecmp(operators->array[(i - 1)/2], "or") == 0) {
            boolean_string[i] = '+';
        } else if (strcasecmp(operators->array[(i - 1)/2], "and") == 0) {
            boolean_string[i] = '*';
        } else {
            fprintf(stderr, "Error: Invalid operator %s\n", operators->array[i]);
            return -1;
        }
    }

    printf("boolean_String: %s\n", boolean_string);

    StringArray boolean_expression = expression_to_string_list(boolean_string);

    OperationTree boolean_tree = build_tree(boolean_expression);
    bool result = (bool)evaluate_boolean_tree(boolean_tree->root);
    
    printf("logical_result: %d\n", result);
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
    printf("records_size: %d\n", table_size);

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
            print_record(where_clause->table, record);
            record_count++;
            if (record_count == 1) {
                selected_records = malloc(sizeof(union record_item *) * record_count);
            } 
            else {
                selected_records = realloc(selected_records, sizeof(union record_item *) * record_count);
            }
            selected_records[record_count - 1] = record;
        }

    }
    printf("\n");
    print_record(where_clause->table, selected_records[0]);
    print_record(where_clause->table, selected_records[1]);
    print_record(where_clause->table, selected_records[2]);
    return record_count;
}

StringArray condition_to_expression(union record_item *record, char *condition, Table table) {
    char *temp = strdup(condition);
    char *attribute_name = strtok(temp, " ");
    //printf("attribute_name: \"%s\"\n", attribute_name);

    Attr attribute = get_attr_by_name(table, attribute_name);
    int data_position = get_attr_position(attribute);
    Type data_type = get_attr_type(attribute);

    //fprintf(stdout, "%d\n", data_position);
    //fprintf(stdout, "%s\n", condition);
  
    union record_item item = record[data_position];

    StringArray expression = expression_to_string_list(condition);
    char* string_record_item = record_item_to_string(data_type, item);
    remove_spaces(string_record_item);
    expression->array[0] = string_record_item;

    free(temp);
    return expression;
}

bool does_record_satisfy_condition(union record_item *record, char *condition, Table table) {
    StringArray expression = condition_to_expression(record, condition, table);
    
    printf("expression: { ");
    for(int i = 0; i < expression->size; i++) {
        if(i == expression->size - 1) {
            printf("%s ", expression->array[i]);
        }
        else {
            printf("%s,", expression->array[i]);
        }
    }
    printf("}\n");

    OperationTree tree = build_tree(expression);
    bool condition_satisfied = determine_conditional(tree->root);
    printf("determine_conditional_result: %d\n", condition_satisfied);

    freeOperationTree(tree);
    return condition_satisfied;
}