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
            for (int i = 0; i < clause->clause_count; i++) {
                free(clause->clauses[i]);
            }
            free(clause->clauses);
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
    clause->array_size = 1;
//    clause->attribute_count = 0;
//    clause->attributes = malloc(sizeof(struct Attribute));
    clause->clauses = malloc(sizeof(char *) * (clause->array_size));

    return clause;
}

double calculate_value(Clause clause, char **math_expression, union record_item **record) {
    Attribute attribute1 = NULL;
    Attribute attribute2 = NULL;
    double value1 = 0.0, value2 = 0.0;
    char *val1pointer, *val2pointer;
    int operator;
    int size = get_array_size(math_expression);

    // ex bar = 3 + bar
    //    bar = foo
    //    bar = bar + 3
    //    bar = foo + bar

    // check to see if a column value was passed in
    attribute1 = get_attribute_from_table(clause->table, math_expression[2]);
    if (attribute1 != NULL) {
        if (attribute1->type == INTEGER) {
            value1 = record[INTEGER]->i;
        } else if (attribute1->type == DOUBLE) {
            value1 = record[DOUBLE]->d;
        } else { // error if char or varchar. todo: error if bool?
            return DBL_MAX;
        }
    } else { // couldn't find attribute in table, assume regular value
        // todo: check if string here    if string -> error?
        value1 = strtod(math_expression[2], &val1pointer);
    }

    if (size == 3) { // ex, bar = 3, or bar = foo
        return value1;
    }

    operator = get_operation(*math_expression[3]);

    attribute2 = get_attribute_from_table(clause->table, math_expression[4]);
    if (attribute2 != NULL) {
        if (attribute2->type == INTEGER) {
            value2 = record[INTEGER]->i;
        } else if (attribute2->type == DOUBLE) {
            value2 = record[DOUBLE]->d;
        } else { // error if char or varchar. todo: error if bool?
            return DBL_MAX;
        }
    } else {
        // todo: check if string here    if string -> error?
        value2 = strtod(math_expression[2], &val2pointer);
    }

    switch (operator) {
        double test;
        case ADDITION:
            test = (value1 + value2);
            return test;
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

        // increase array size by 1
        set_clause->array_size++;
        set_clause->clauses = realloc(set_clause->clauses, sizeof(char *) * (set_clause->array_size));

        // copy clause to array
        set_clause->clauses[set_clause->clause_count] = malloc(strlen(clause) + 1);
        strcpy(set_clause->clauses[set_clause->clause_count++], clause);

        clause = strtok(NULL, ",");
    }

    return set_clause;
}

// todo
Clause parse_where_clause(char *clauses) {
    // accepted keywords: AND, OR, NOT

    Clause where_clause = create_clause();

    return where_clause;
}