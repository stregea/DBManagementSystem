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
//            for (int i = 0; i < clause->clause_count; i++) {
//                free(clause->clauses[i]);
//            }
            free_string_array(clause->clauses);
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
    clause->clause_count = 0;
//    clause->attribute_count = 0;
//    clause->attributes = malloc(sizeof(struct Attribute));
    clause->clauses = create_string_array();

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
        while (clause[0] == ' ') {
            clause++;
        }
        int last = strlen(clause) - 1;
        while (clause[last] == ' ') {
            clause[last] = '\0';
            last--;
        }

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

    return where_clause;
}