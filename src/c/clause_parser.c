#include "../headers/clause_parser.h"
#include "../headers/arrays.h"
#include "../headers/Enums.h"
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>

void free_clause(Clause clause) {
    if (clause != NULL) {
        if (clause->clauses != NULL) {
            free_string_array(clause->clauses);
        }

        if(clause->operators != NULL){
            free_string_array(clause->operators);
        }
//        if(clause->attributes != NULL){
//            // NOTE: no need to free the pointers here. freeTable will handle it.
//            free(clause->attributes);
//        }

        free(clause);
    }
}

Clause create_clause() {
    Clause clause = malloc(sizeof(struct Clause));
    clause->clauses = create_string_array();
    clause->operators = create_string_array();

    return clause;
}

double calculate_value(Clause clause, StringArray math_expression, union record_item *record) {
    Attribute attribute1 = NULL;
    Attribute attribute2 = NULL;
    double value1 = 0.0, value2 = 0.0;
    char *val1pointer, *val2pointer;
    int operator;

    // ex bar = 3 + bar
    //    bar = foo
    //    bar = bar + 3
    //    bar = foo + bar

    // check to see if a column value was passed in
    attribute1 = get_attribute_from_table(clause->table, math_expression->array[2]);
    if (attribute1 != NULL) {
        if (attribute1->type == INTEGER || attribute1->type == DOUBLE) {
            int i = get_attribute_index(clause->table, attribute1);

            if(i == -1){
                return DBL_MAX;
            }

            if(attribute1->type == INTEGER){
                value1 = record[i].i;
            }
            else{ // otherwise a double
                value1 = record[i].d;
            }

        } else {
            return DBL_MAX;
        }
    } else { // couldn't find attribute in table, assume regular value
        // todo: check if string here    if string -> error?
        value1 = strtod(math_expression->array[2], &val1pointer);
    }

    if (get_array_size(math_expression) == 3) { // ex, bar = 3, or bar = foo
        return value1;
    }

    operator = get_operation(*math_expression->array[3]);

    attribute2 = get_attribute_from_table(clause->table, math_expression->array[4]);
    if (attribute2 != NULL) {
        if (attribute2->type == INTEGER || attribute2->type == DOUBLE) {

            int i = get_attribute_index(clause->table, attribute2);

            if(i == -1){
                return DBL_MAX;
            }

            if(attribute2->type == INTEGER){
                value2 = record[i].i;
            }
            else{ // otherwise a double
                value2 = record[i].d;
            }
        } else {
            return DBL_MAX;
        }
    } else {
        // todo: check if string here    if string -> error?
        value2 = strtod(math_expression->array[4], &val2pointer);
    }

    switch (operator) {
        case ADDITION:
            return value1 + value2;
        case SUBTRACTION:
            return value1 - value2;
        case MULTIPLICATION:
            return value1 * value2;
        case DIVISION:
            return value1 / value2;
        default:
            return DBL_MAX;
    }

}

char* clean_clause(char* clause){
    char *tmp = clause;
    while (tmp[0] == ' ') {
        tmp++;
    }
    int last = strlen(tmp) - 1;
    while (tmp[last] == ' ') {
        tmp[last] = '\0';
        last--;
    }
    return tmp;
}

// TODO - implement attributes from Clause??
Clause parse_set_clause(char *clauses) {
    Clause set_clause = NULL;

    char *temp_clauses = malloc(strlen(clauses) + 1);
    strcpy(temp_clauses, clauses);

    char *clause = strtok(temp_clauses, ",");

    if (clause != NULL) {
        set_clause = create_clause();
    }

    while (clause != NULL) {
        // remove any misleading spaces

        clause = clean_clause(clause);

        set_clause->clauses->array = realloc(set_clause->clauses->array, sizeof(char *) * (set_clause->clauses->size + 1));

        // copy clause to array
        set_clause->clauses->array[set_clause->clauses->size] = malloc(strlen(clause) + 1);
        strcpy(set_clause->clauses->array[set_clause->clauses->size++], clause);

        clause = strtok(NULL, ",");
    }

    free(temp_clauses);
    return set_clause;
}

// todo
Clause parse_where_clause(char *clauses) {
    // accepted keywords: AND, OR, NOT

    Clause where_clause = create_clause();
    where_clause->operators = create_string_array();

    char* condition = strdup(clauses);

    char* token = strtok(condition, " ");

    // determine the difference kinds of logical operators found in string
    while(token != NULL){
        if(strcasecmp(token, "and") == 0 || strcasecmp(token, "or") == 0){
            where_clause->operators->array = realloc(where_clause->operators->array, sizeof(char *) * (where_clause->operators->size + 1));
            where_clause->operators->array[where_clause->operators->size] = malloc(strlen(token) + 1);

            strcpy(where_clause->operators->array[where_clause->operators->size], token);
            where_clause->operators->size++;
        }
        token = strtok(NULL, " ");
    }
    printf("logical_operator_size: %d\n", where_clause->operators->size);

    printf("line hit\n");

    free(condition);
    condition = strdup(clauses);
    printf("condition: %s\n", condition);

    // build the where clause
    for(int i = 0; i < where_clause->operators->size; i++){

        if(i != 0){
            condition = token;
        }

        token = strstr(condition, where_clause->operators->array[i]);

        if (token == NULL){
            free(condition);
            return NULL;
        }

        *token = '\0';
        token = token + strlen(where_clause->operators->array[i]);
        printf("op: [%s]\n", where_clause->operators->array[i]);

        condition = clean_clause(condition);
        printf("condition: [%s]\n", condition);
        where_clause->clauses->array = realloc(where_clause->clauses->array, sizeof(char*) * (where_clause->clauses->size + 1));
        where_clause->clauses->array[where_clause->clauses->size] = malloc(strlen(condition) + 1);

        strcpy(where_clause->clauses->array[where_clause->clauses->size], condition);
        where_clause->clauses->size++;

        if(i == where_clause->operators->size - 1){ // strstr makes us do this
            token = clean_clause(token);

            printf("condition: [%s]\n", token);
            where_clause->clauses->array = realloc(where_clause->clauses->array, sizeof(char*) * (where_clause->clauses->size + 1));
            where_clause->clauses->array[where_clause->clauses->size] = malloc(strlen(token) + 1);

            strcpy(where_clause->clauses->array[where_clause->clauses->size], token);
            where_clause->clauses->size++;
        }

    }
    printf("**************************************************\n");
    for(int i = 0; i < where_clause->clauses->size; i++) {
        printf("\"%s\"\n", where_clause->clauses->array[i]);
        //does_record_satisfy_condition(where_clause->clauses->array[i]);
    }
    for(int i = 0; i < where_clause->operators->size; i++) {
        printf("\"%s\"\n\n", where_clause->operators->array[i]);
    }
    free(condition);
    return where_clause;
}