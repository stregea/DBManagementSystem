#include "../headers/dml_manager.h"
#include "../headers/Enums.h"
#include "../headers/catalog.h"
#include "../headers/storagemanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void freeRecord(Table table, union record_item *record) {a
    if (record != NULL) {
        free(record);
    }
}

/**
 * Print out a record.
 *
 * Note: This is for testing purposes.
 */
void print_record(Table table, union record_item *record) {
    printf("(");
    for (int i = 0; i < table->attribute_count; i++) {
        char * extra_space = " ";

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
            if (strlen(value) != attribute->size) {
                fprintf(stderr, "Error: %s size must be equal to %d.\n", value, attribute->size);
                strcpy(recordItem.c, ""); // not sure if this is how we want to handle this?
                return recordItem;
            }
            strcpy(recordItem.c, value);
            break;
        case VARCHAR:
            if (strlen(value) > attribute->size) {
                fprintf(stderr, "Error: %s size must be <= to %d.\n", value, attribute->size);
                strcpy(recordItem.v, ""); // not sure if this is how we want to handle this?
                return recordItem;
            }
            strcpy(recordItem.v, value);
            break;
        default:
            break;
    }
    return recordItem;
}

// TODO: test
// TODO: if NULL, return -1 in function that this is called in or simply ignore null value?.
// Note: each tuple will be sent in individually.
// tuple examples: (1 "foo" true 2.1)
//                 (3 "baz true 4.14)
//                 (2 "bar" false 5.2)
union record_item *create_record_from_statement(Table table, char *tuple) {

    union record_item * record = malloc(sizeof(union record_item) * table->attribute_count);

    char *temp = malloc(strlen(tuple) + 1);
    strcpy(temp, tuple);
    char *next_value = strtok(temp, " ()");

    int attribute_counter = 0;
    while(next_value != NULL || attribute_counter < table->attribute_count){ // TODO: have it be &&?

        union record_item recordItem = create_record_item(table->attributes[attribute_counter], next_value);

//        if(recordItem.i == INVALID){ // there was an error
//            freeRecord(table, record);
//            free(temp);
//            return NULL;
//        }

        record[attribute_counter] = recordItem;
        attribute_counter++;
        next_value = strtok(NULL, " ()");
    }
    free(temp);
    return record;
}

// TODO
int parse_insert_statement(char *statement) {
    /**
     * Examples:
     *   insert into foo values (1 "foo" true 2.1);
     *
     *   insert into foo values (1 "foo" true 2.1),
     *                          (3 "baz" true 4.14),
     *                          (2 "bar" false 5.2);
     */

    //TODO: come up with a better parsing way than this.
    char* tokenizer = strtok(statement, " "); // "insert"

    tokenizer = strtok(NULL, " ");
    if(tokenizer != NULL && strcasecmp(tokenizer, "into") == 0){

        char* table_name = strtok(NULL, " "); // table name
        if(table_name != NULL){

            // Note: No need to free this table since it will
            // be free'd upon termination of application.
            Table table = get_table_from_catalog(table_name);

            if(table == NULL){ // table doesn't exist
                fprintf(stderr, "Error: Table %s does not exist.\n", table_name);
                return -1;
            }

            tokenizer = strtok(NULL, " ");
            if(tokenizer != NULL && strcasecmp(tokenizer, "values") == 0){

                tokenizer = strtok(NULL, ";");
                char* tuples = malloc(strlen(tokenizer)+1);
                strcpy(tuples, tokenizer);

                char* tuple_token;
                char * tuple = strtok_r(tuples, ",", &tuple_token);
                while(tuple != NULL){

                    union record_item* record = create_record_from_statement(table, tuple);
                    if(record != NULL){
                        //NOTE: THIS IS FOR TESTING ONLY
                        print_record(table, record);
                        // insert the tuple into the storage manager
//                        insert_record(table->tableId, record);
                        freeRecord(table, record);
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

// TODO
int parse_update_statement(char *statement) { return 0; }

// TODO
int parse_delete_from_statement(char *statement) { return 0; }

// TODO
int parse_select_statement(char *statement) { return 0; }