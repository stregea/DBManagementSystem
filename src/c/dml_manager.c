#include "../headers/dml_manager.h"
#include "../headers/Enums.h"
#include "../headers/catalog.h"
#include "../headers/storagemanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Retrieve the index of the first occurrence of a word within a string.
 * @param str - The string to search.
 * @param word - The word to search for.
 * @return -1 on error, otherwise the index at which the word first occurs.
 */
int get_index_of_word_from_string(const char* string, const char* word){
    int i, j , flag;

    i = 0;
    while(string[i] != '\0')
    {
        if(string[i] == word[0])
        {
            flag = 1;
            j = 0;
            while(word[j] != '\0')
            {
                if(string[i + j] != word[j])
                {
                    flag = 0;
                    break;
                }
                j++;
            }
        }
        if(flag == 1)
        {
            break;
        }
        i++;
    }
    if(flag == 0)
    {
        return -1;
    }
    else
    {
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

union record_item create_record_item(Attribute attribute, char *value) {
    union record_item recordItem;

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
            recordItem.i = atoi(value);
            break;
        case DOUBLE:
            recordItem.d = atof(value);
            break;
        case BOOL:
            if (strcasecmp(value, "true") == 0) { // i'm not too sure about this one.
                recordItem.b[0] = true;
                recordItem.b[1] = true;
            } else {
                recordItem.b[0] = false;
                recordItem.b[1] = false;
            }
            break;
        case CHAR:
            if (strlen(value) != string_size) {
                fprintf(stderr, "Error: %s's length must be equal to %d.\n", value, attribute->size);
                strcpy(recordItem.c, "ERROR"); // not sure if this is how we want to handle this?
                return recordItem;
            }
            strcpy(recordItem.c, value);
            break;
        case VARCHAR:
            if (strlen(value) > string_size) {
                fprintf(stderr, "Error: %s's length must be <= to %d.\n", value, attribute->size);
                strcpy(recordItem.v, "ERROR"); // not sure if this is how we want to handle this?
                return recordItem;
            }
            strcpy(recordItem.v, value);
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
    char *next_value = strtok(temp, " ()");

    int attribute_counter = 0;
    while (next_value != NULL || attribute_counter < table->attribute_count) { // TODO: have it be &&?

        union record_item recordItem = create_record_item(table->attributes[attribute_counter], next_value);

        // Error with char creation or varchar creation.
        if ((table->attributes[attribute_counter]->type == CHAR && strcasecmp(recordItem.c, "ERROR") == 0) ||
            (table->attributes[attribute_counter]->type == VARCHAR && strcasecmp(recordItem.v, "ERROR") == 0)) {
            free(temp);
            freeRecord(record);
            return NULL;
        }

        record[attribute_counter] = recordItem;
        attribute_counter++;
        next_value = strtok(NULL, " ()");
    }
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
                        }

                        freeRecord(record);
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

void free_clause(char **clause, int num_clauses) {
    if (clause != NULL) {
        for (int i = 0; i < num_clauses; i++) {
            if (clause[i] != NULL) {
                free(clause[i]);
            }
        }
        free(clause);
    }
}

char **parse_set_clause(char *clauses) {
    size_t array_size = 1;
    char **clause_list = malloc(sizeof(char*) * array_size);

    char *temp_clauses = malloc(strlen(clauses) + 1);
    strcpy(temp_clauses, clauses);

    char *clause = strtok(temp_clauses, ",");

    int clause_count = 0;
    while (clause != NULL) {
        // remove any misleading spaces
        while(clause[0] == ' '){
            clause++;
        }
        int last = strlen(clause) - 1;
        while(clause[last] == ' '){
            clause[last] = '\0';
            last--;
        }

        // increase array size by 1
        array_size++;
        clause_list = realloc(clause_list, sizeof(char*) * array_size);

        // copy clause to array
        clause_list[clause_count] = malloc(strlen(clause) + 1);
        strcpy(clause_list[clause_count++], clause);

        clause = strtok(NULL, ",");
    }

    return clause_list;
}

// todo
char **parse_where_clause(char *clauses) {
    // accepted keywords: AND, OR, NOT

    size_t array_size = 1;
    char **clause_list = malloc(array_size);

    return clause_list;
}

// TODO
int parse_update_statement(char *statement) {
    char* temp_statement = malloc(strlen(statement) + 1);
    strcpy(temp_statement, statement);

    char *tokenizer = strtok(temp_statement, " "); // "update"

    if (tokenizer != NULL && strcasecmp(tokenizer, "update") == 0) {
        char *table_name = strtok(NULL, " ");
        if (table_name != NULL) {

            Table table = get_table_from_catalog(table_name);
            char **set_clause;
            char **where_clause;

            // parse set
            tokenizer = strtok(NULL, " ");
            if (tokenizer != NULL && strcasecmp(tokenizer, "set") == 0) {
                tokenizer = strtok(NULL, ";");

                // add '|' to allow for parsing the set clause from the where clause.
                tokenizer[get_index_of_word_from_string(tokenizer, "where")-1] = '|';
                tokenizer = strtok(tokenizer, "|");

                if(tokenizer != NULL){
                    set_clause = parse_set_clause(tokenizer);

//                    tokenizer = strtok(NULL, " ");
//                    if(tokenizer != NULL){
//                        where_clause = parse_where_clause(tokenizer);
//                    }
                    // parse where clause
                }
            }


        }
    }
    free(temp_statement);
    // bad keyword
    return -1;
}

// TODO
int parse_delete_from_statement(char *statement) { return 0; }

// TODO
int parse_select_statement(char *statement) { return 0; }