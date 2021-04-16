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

double eval(Node node);
#endif //DBMANAGEMENTSYSTEM_SHUNTING_YARD_PARSER_H
