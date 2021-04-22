#include "../headers/node.h"
#include "../headers/arrays.h"
#include "../headers/clause_parser.h"
#ifndef DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
#define DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
/**
 * Build a parse tree from an expression.
 * @param expression - The expression to convert to a tree.
 * @return a tree.
 */
OperationTree build_tree(StringArray expression);

double evaluate_tree(Node node);

bool determine_conditional(Node node);

int precedence(char *operation);
StringArray expression_to_string_list(char *expression);
StringArray infix_to_postfix(StringArray expression);
bool record_satisfies_where(Clause where_clause, union record_item *record);
bool is_character_operator(char character);
#endif //DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
