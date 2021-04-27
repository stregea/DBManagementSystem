#include "../headers/clause_parser.h"
#include "../headers/arrays.h"
#include "../headers/Enums.h"
#include "../headers/table.h"
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
    Attr attribute1 = NULL;
    Attr attribute2 = NULL;
    double value1 = 0.0, value2 = 0.0;
    char *val1pointer, *val2pointer;
    int operator;

    // check to see if a column value was passed in
    attribute1 = get_attr_by_name(clause->table, math_expression->array[2]);
    if (attribute1 != NULL) {
        if (attribute1->type->type_num == INTEGER || attribute1->type->type_num == DOUBLE) {
            int i = attribute1->position;

            if(i == -1){
                fprintf(stderr, "Error: invalid position in table\n");
                return DBL_MAX;
            }

            if(attribute1->type->type_num == INTEGER){
                value1 = record[i].i;
            }
            else{ // otherwise a double
                value1 = record[i].d;
            }

        } else {
            fprintf(stderr, "Error: invalid value passed in to calculate new value for '%s'\n", attribute1->name);
            return DBL_MAX;
        }
    } else { // couldn't find attribute in table, assume regular value
        value1 = strtod(math_expression->array[2], &val1pointer);
    }

    if (get_array_size(math_expression) == 3) { // ex, bar = 3, or bar = foo
        return value1;
    }

    operator = get_operation(*math_expression->array[3]);

    attribute2 = get_attr_by_name(clause->table, math_expression->array[4]);
    if (attribute2 != NULL) {
        if (attribute2->type->type_num == INTEGER || attribute2->type->type_num == DOUBLE) {

            int i = attribute2->position;

            if(i == -1){
                fprintf(stderr, "Error: invalid position in table\n");
                return DBL_MAX;
            }

            if(attribute2->type->type_num == INTEGER){
                value2 = record[i].i;
            }
            else{ // otherwise a double
                value2 = record[i].d;
            }
        } else {
            fprintf(stderr, "Error: invalid value passed in to calculate new value for '%s'\n", attribute2->name);
            return DBL_MAX;
        }
    } else {
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
    // accepted keywords: AND, OR

    Clause where_clause = create_clause();
    where_clause->operators = create_string_array();
    where_clause->clauses = create_string_array();
    char* condition = strdup(clauses);

    char* token = strtok(condition, " ");
    const char *or = " or ";
    const char *and = " and ";

    // determine the different kinds of logical operators found in string
    while(token != NULL){
        if(strcasecmp(token, "and") == 0 || strcasecmp(token, "or") == 0){
            where_clause->operators->array = realloc(where_clause->operators->array, sizeof(char *) * (where_clause->operators->size + 1));
            where_clause->operators->array = realloc(where_clause->operators->array, sizeof(char *) * (where_clause->operators->size + 1));
            where_clause->operators->array[where_clause->operators->size++] = strdup(token);
        }
        token = strtok(NULL, " ");
    }


    free(condition);
    condition = strdup(clauses);

    // build the where clause
    if(where_clause->operators->size == 0){ // if only one condition.
        condition = clean_clause(condition);

        where_clause->clauses->array = realloc(where_clause->clauses->array, sizeof(char*) * (where_clause->clauses->size + 1));
        where_clause->clauses->array[where_clause->clauses->size++] = strdup(condition);
    }else{
        for(int i = 0; i < where_clause->operators->size; i++){

            if(i != 0){
                condition = token;
            }

            // currently splitting on "or" and "and" needs to be " or " and " and "
            if(strcasecmp(where_clause->operators->array[i], "and") == 0) {
                token = strstr(condition, and);
            }
            else if(strcasecmp(where_clause->operators->array[i], "or") == 0) {
                token = strstr(condition, or);
            }

            if (token == NULL){
                free(condition);
                return NULL;
            }

            *token = '\0';
            token = token + strlen(where_clause->operators->array[i]) + 2;
            //printf("op: [%s]\n", where_clause->operators->array[i]);

            condition = clean_clause(condition);
            //printf("condition: [%s]\n", condition);
            where_clause->clauses->array = realloc(where_clause->clauses->array, sizeof(char*) * (where_clause->clauses->size + 1));
            where_clause->clauses->array[where_clause->clauses->size++] = strdup(condition);

            if(i == where_clause->operators->size - 1){ // strstr makes us do this
                token = clean_clause(token);
                //printf("condition: [%s]\n", token);
                where_clause->clauses->array = realloc(where_clause->clauses->array, sizeof(char*) * (where_clause->clauses->size + 1));
                where_clause->clauses->array[where_clause->clauses->size++] = strdup(token);
            }
        }
    }

    if(condition != NULL){
        //free(condition);
    }

    return where_clause;
}