#include "../headers/node.h"
#include "../headers/arrays.h"
#ifndef DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
#define DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
/**
 * Build a parse tree from a string in post-fix notation.
 * @param string - The post-fix notation string.
 * @return a tree.
 */
OperationTree build_tree(StringArray string);

double evaluate_tree(Node node);

bool determine_conditional(Node node);

int precedence(char *operation);
StringArray expression_to_string_list(char *expression);
StringArray infix_to_postfix(StringArray expression);
#endif //DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
