#include "../headers/dml_manager.h"
#include "../headers/Enums.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void freeRecord(union record_item *record) {
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

union record_item *create_record_item(Attribute attribute, int type, char *value) {
    union record_item *recordItem = malloc(sizeof(union record_item));
    switch (type) {
        case INTEGER:
            recordItem->i = atoi(value);
            break;
        case DOUBLE:
            recordItem->d = atof(value);
            break;
        case BOOL:
            if (strcasecmp(value, "true") == 0) { // i'm not too sure about this one.
                recordItem->b[0] = true;
                recordItem->b[1] = true;
            } else {
                recordItem->b[0] = false;
                recordItem->b[1] = false;
            }
            break;
        case CHAR:
            if (strlen(value) != attribute->size) {
                fprintf(stderr, "Error: %s size must be equal to %d.\n", value, attribute->size);
                free(recordItem);
                return NULL;
            }
            strcpy(recordItem->c, value);
            break;
        case VARCHAR:
            if (strlen(value) > attribute->size) {
                fprintf(stderr, "Error: %s size must be of <= to %d.\n", value, attribute->size);
                free(recordItem);
                return NULL;
            }
            strcpy(recordItem->v, value);
            break;
        default:
            free(recordItem);
            return NULL;
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

        record[attribute_counter] = *create_record_item(table->attributes[attribute_counter], INTEGER, next_value);

        //NOTE: THIS IS FOR TESTING ONLY
        print_record(table, record);

        attribute_counter++;
        next_value = strtok(NULL, " ()");
    }

    free(temp);
    return record;
}

// TODO
int parse_insert_statement(char *statement) { return 0; }

// TODO
int parse_update_statement(char *statement) { return 0; }

// TODO
int parse_delete_from_statement(char *statement) { return 0; }

// TODO
int parse_select_statement(char *statement) { return 0; }