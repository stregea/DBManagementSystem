#ifndef DBMANAGEMENTSYSTEM_CLAUSE_PARSER_H
#define DBMANAGEMENTSYSTEM_CLAUSE_PARSER_H
#include "table.h"
#include "storagemanager.h"
#include "arrays.h"

struct Clause {
    int clause_count; // the total count of clauses.
//    int attribute_count; // todo (may not need): counter to keep track of the total attributes specified.
//    Attribute * attributes; // todo (may not need): Array of attributes that point to a tables particular attribute.
    StringArray clauses; // 2-D array of strings that at each row will contain each specified clause
    Table table;
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
 * Calculate a value from an array containing a mathematical expression.
 * @param clause - The clause that contains table information.
 * @param math_expression - The array containing the mathematical expression.
 * @param record - The record to use in the case the expression uses a table attribute.
 * @return A value based on the math expression passed in.
 */
double calculate_value(Clause clause, StringArray math_expression, union record_item * record);
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
