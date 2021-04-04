#ifndef DBMANAGEMENTSYSTEM_CLAUSE_PARSER_H
#define DBMANAGEMENTSYSTEM_CLAUSE_PARSER_H

struct Clause {
    int array_size; // the size of the clause array.
    int clause_count; // the total count of clauses.
    char **clauses; // 2-D array of strings that at each row will contain each specified clause
};
typedef struct Clause *Clause;

/**
 * Free a clause from memory.
 * @param clause - The clause to free.
 * @param num_clauses
 */
void free_clause(Clause clause);

/**
 * Create an instance of a clause.
 * @return A new clause.
 */
Clause create_clause();

/**
 * Parse the set clause into a Clause struct.
 * ex: set bar = bar + 1, baz = 1.1
 * @param clauses
 * @return
 */
Clause parse_set_clause(char *clauses);

/**
 * Parse the where clause into a Clause struct.
 * ex: where a = "foo" AND bar > 2
 * @param clauses
 * @return
 */
Clause parse_where_clause(char *clauses);

#endif
