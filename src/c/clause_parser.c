#include "../headers/clause_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_clause(Clause clause) {
    if (clause != NULL) {
        if(clause->clauses != NULL){
            for(int i = 0; i < clause->clause_count; i++){
                free(clause->clauses[i]);
            }
            free(clause->clauses);
        }

        if(clause->attributes != NULL){
            // NOTE: no need to free the pointers here. freeTable will handle it.
            free(clause->attributes);
        }

        free(clause);
    }
}

Clause create_clause(){
    Clause clause = malloc(sizeof(struct Clause));
    clause->clause_count = 0;
    clause->array_size = 1;
    clause->attribute_count = 0;
    clause->attributes = malloc(sizeof(struct Attribute));
    clause->clauses = malloc(sizeof(char*) * (clause->array_size));

    return clause;
}

// TODO - implement attributes from Clause??
Clause parse_set_clause(char *clauses) {
    Clause set_clause = create_clause();

    char *temp_clauses = malloc(strlen(clauses) + 1);
    strcpy(temp_clauses, clauses);

    char *clause = strtok(temp_clauses, ",");

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
        set_clause->array_size++;
        set_clause->clauses = realloc(set_clause->clauses, sizeof(char*) * (set_clause->array_size));

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