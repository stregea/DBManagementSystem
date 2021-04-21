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
                printf("%s%s", (record[i].b[0] == 1) ? "true" : "false", extra_space); // not to sure by bool is an array
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

                        if (includes_where) {
//                            where->
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

                                        if(strcasecmp(tmp_clause->array[2], "null") == 0 && attribute->notnull){
                                            fprintf(stderr,"Error: Cannot insert null into attribute %s since notnull was specified.\n", attribute_name);
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
                                            char* value = tmp_clause->array[2];
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
                                                if (res == DBL_MAX ) {
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
                                                if(attribute2 != 0 && attribute2->type->type_num == BOOL){
                                                    record[record_index].b[0] = record[attribute2->position].b[0];
                                                }
                                                else if(strcasecmp(tmp_clause->array[2], "true") == 0
                                                    || strcasecmp(tmp_clause->array[2], "false") == 0
                                                    || strcasecmp(tmp_clause->array[2], "null") == 0 ){

                                                    if(strcasecmp(tmp_clause->array[2], "true") == 0){
                                                        record[record_index].b[0] = true;
                                                    }
                                                    if(strcasecmp(tmp_clause->array[2], "false") == 0){
                                                        record[record_index].b[0] = NULL;
                                                    }
                                                    else{
                                                        record[record_index].b[0] = false;
                                                    }
                                                }else{
                                                    fprintf(stderr, "Error: Invalid value for %s.\n", tmp_clause->array[2]);
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
                                                if(attribute2 != 0 && attribute2->type->type_num == CHAR){

                                                    string_size_attribute2 = attribute2->type->num_chars;

                                                    // If '"' is on both ends of the string, add 2 to the allowed size of the string
                                                    // since, they don't technically count towards the total size of the string.
                                                    char* value2 = record[attribute2->position].c;
                                                    if (value2[0] == '"' && value2[strlen(value2) - 1] == '"') {
                                                        string_size_attribute2 += 2;
                                                    }

                                                    // determine if attribute 2 meets the size constraints for attribute 1
                                                    if(strlen(record[attribute2->position].c) != string_size_attribute2){
                                                        fprintf(stderr, "Error: %s's length must be equal to %d.\n", attribute2->name, attribute->type->num_chars);
                                                        free_string_array(tmp_clause);
                                                        free_clause(set);
                                                        free(set_clause);
                                                        free_string_array(statement_array);
                                                        free_table_from_storagemanager(table_size, storagemanager_table);
                                                        return -1;
                                                    }

                                                    strcpy(record[record_index].c, record[attribute2->position].c);
                                                }
                                                else if (strcasecmp(tmp_clause->array[2], "null") == 0 || strlen(tmp_clause->array[2]) == string_size) {
                                                    strcpy(record[record_index].c, tmp_clause->array[2]);
                                                } else { // error
                                                    fprintf(stderr, "Error: %s's length must be equal to %d.\n", tmp_clause->array[2], attribute->type->num_chars);
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

                                                if(attribute2 != 0 && attribute2->type->type_num == VARCHAR){

                                                    string_size_attribute2 = attribute2->type->num_chars;

                                                    // If '"' is on both ends of the string, add 2 to the allowed size of the string
                                                    // since, they don't technically count towards the total size of the string.
                                                    char* value2 = record[attribute2->position].c;
                                                    if (value2[0] == '"' && value2[strlen(value2) - 1] == '"') {
                                                        string_size_attribute2 += 2;
                                                    }

                                                    // determine if attribute 2 meets the size constraints for attribute 1
                                                    if(strlen(record[attribute2->position].c) > string_size_attribute2){
                                                        fprintf(stderr, "Error: %s's length must be equal to %d.\n", attribute2->name, attribute->type->num_chars);
                                                        free_string_array(tmp_clause);
                                                        free_clause(set);
                                                        free(set_clause);
                                                        free_string_array(statement_array);
                                                        free_table_from_storagemanager(table_size, storagemanager_table);
                                                        return -1;
                                                    }

                                                    strcpy(record[record_index].c, record[attribute2->position].c);
                                                }
                                                else if (strcasecmp(tmp_clause->array[2], "null") == 0 || strlen(tmp_clause->array[2]) <= string_size) {
                                                    strcpy(record[record_index].v, tmp_clause->array[2]);
                                                } else { // string size isn't correct
                                                    fprintf(stderr, "Error: %s's length must be less than or equal to %d.\n", tmp_clause->array[2], attribute->type->num_chars);
                                                    free_string_array(tmp_clause);
                                                    free_clause(set);
                                                    free(set_clause);
                                                    free_string_array(statement_array);
                                                    free_table_from_storagemanager(table_size, storagemanager_table);
                                                    return -1;
                                                }
                                                break;
                                        }

                                        // update the record and check for any errors while updating
                                        if(update_record(table->num, record) == -1) {
                                            fprintf(stderr, "An error has occurred in an attempt to update a record.\n");
                                            free_string_array(tmp_clause);
                                            free_clause(set);
                                            free(set_clause);
                                            free_string_array(statement_array);
                                            free_table_from_storagemanager(table_size, storagemanager_table);
                                            return -1;
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

    if(token == NULL && strcasecmp(token, "from") != 0) {
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

    if(token == NULL && strcasecmp(token, "where") != 0) {
        fprintf(stderr, "Error: expected \"where\" got %s\n", token);
        return -1;
    }

    union record_item **records = NULL;
    int table_size = get_records(table->num, &records);

    if(table_size == -1) {
        fprintf(stderr, "Error: unable to records from table %s\n", table_name);
        return -1;
    }

    // conditionals
    char *condition = strtok(NULL, ";");
    printf("condition: %s\n", condition);
    
    // parse where clause
    Clause where_clause = parse_where_clause(condition);

    printf("clauses: %s\n", where_clause->clauses->array[1]);

    free(where_clause);

    /**
    if(selected_records == NULL) {
        // do not consider this an error
        printf("Unable to find records that satisfy condition: %s\n", condition);
        return 0;
    }

    
    union record_item *record = records[0];
    union record_item *key_values = malloc(table->key_indices_count * sizeof(union record_item*));
    
    int key_index;
    for(int i = 0; i < table->key_indices_count; i++) {
        key_index = table->key_indices[i];
        key_values[i] = record[i];
    }
    
    int remove_result = remove_record(table->tableId, key_values);
    // print all records
    for(int i = 0; i < table_size; i++) {
        print_record(table, records[i]);
    }
    */
    return 0; 
 }

// TODO
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
    if (statement_array->array[index] != NULL && strcasecmp(statement_array->array[index++], "select") == 0) {

        //check for where clause, will determine how we find the from clause
        bool includes_where = false;
        if (get_index_of_word_from_string(statement, "where") >= 0) {
            includes_where = true;
        }
        char *from_clause = NULL;
        if(includes_where){
            from_clause = array_of_tokens_to_string(statement_array, "from", "where", false);
        }
        else{
            from_clause = array_of_tokens_to_string(statement_array, "from", END_OF_ARRAY, false);
        }

        // TODO: Create a reallocable record set to hold final result set.
        // Ideally this should be returned

        char *from_token;
        char *table_name = strtok_r(from_clause, ",", &from_token);
        if (table_name != NULL) { //turn this into a while loop to get multiple tables.
            Table table = get_table_by_name(table_name);

            if (table != NULL) {

                // TODO: Get the whole of the records from one table if it isn't null.
                // if result set isn't empty. cartesian product these records with result set

                char *select_clause = NULL;
                char *where_clause = NULL;
                Clause select = NULL;
                Clause where = NULL;

                // Determine if a where clause exists.

                union record_item **storagemanager_table = NULL;

                int table_size = get_records(table->num, &storagemanager_table);


                // Build the select clause
                // this c
                select_clause = array_of_tokens_to_string(statement_array, "select", "from", false);

                for (int i = 0; i < table_size; i++) {
                    //testing. this could be repurposed into nested for loops for cartesian product
                    union record_item *record = storagemanager_table[i]; // todo: may need to free this
                    print_record(table, record);
                }

                // TODO: determine selected columns
                // get string after "select"
                char * columns = malloc(strlen(select_clause)); // This is longer than we need but who cares
                //TODO need to trim this but trimwhitespace is pretty cursed
                //strcpy(columns, strstr(select_clause, "select") + 6); //This segfaults?
                // check for correct syntax
                // something something strtok
                // get list of columns as strings
                // parse column names to check for periods separating table and column
                // check each table name to see if it exists and is part of the former list of columns
                // check each column name to see if the table has that column

                if (includes_where){
                    //TODO: where clause logic
                }

                free(columns);
                free(from_clause);
                free(select_clause);
                free_string_array(statement_array);
                return 0;
            }
        }
    }
    free_string_array(statement_array);
    // bad keyword
    return -1;
}

bool does_record_satisfy_condition(union record_item *record, char *condition, Table table) {

    size_t condition_length = strlen(condition);
    char *condition_formatted = malloc(condition_length + 2);
    strcpy(condition_formatted, condition);
    condition_formatted[condition_length + 1] = ';';
    condition_formatted[condition_length + 2] = '0';

    char *attribute_name = strtok(condition, " ");
    printf("attribute_name: \"%s\"\n", attribute_name);
    union record_item item;
    for(int i = 0; i < table->attrs_size; i++) {
        if(strcasecmp(table->attrs[i]->name, attribute_name) == 0) {
            item = record[i];
        }
    }

    /*
    if(item == NULL) {
        fprintf(stderr, "Error: no attribute \"%s\" in table\n", attribute_name);
        return false;
    }
    */

    char *operator =  strtok(NULL, " ");
    char *value = strtok(NULL, ";");
    printf("operator: \"%s\"\n", operator);
    printf("value: \"%s\"\n", value);

    int type = get_conditional(operator);
    
    switch(type) {
      case EQUALS :
        break;
      case GREATER_THAN :
        break;
      case GREATER_THAN_OR_EQUAL_TO :
        break;
      case LESS_THAN :
        break;
      case LESS_THAN_OR_EQUAL_TO :
        break;
      case NOT_EQUALS:
        break;
      default :
        fprintf(stderr, "Error: no operator \"%s\" in table\n", operator);
    }
    return true;
}
