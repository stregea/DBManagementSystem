#include "../headers/database.h"
#include "../headers/catalog.h"
#include "../headers/dml_parser.h"
#include "../headers/ddl_parser.h"
#include "../headers/arrays.h"
#include "../headers/Enums.h"
#include "../headers/shunting_yard_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void treeprint(Node root, int level) {
    if (root == NULL)
        return;
    for (int i = 0; i < level; i++)
        printf(i == level - 1 ? "- " : "  ");

    printf("%s\n", root->value);
    treeprint(root->left, level + 1);
    treeprint(root->right, level + 1);
}

int main() {
    char *expression = "a<b";
    char *string = malloc(strlen(expression) + 1);
    strcpy(string, expression);
    StringArray strings = expression_to_string_list(string);
    strings = infix_to_postfix(strings);

    for (int i = 0; i < strings->size; i++) {
        printf("%s ", strings->array[i]);
    }
    printf("\n");
    OperationTree tree = build_tree(strings);

    treeprint(tree->root, 0);

//    double d = evaluate_tree(tree->root);
bool test = determine_conditional(tree->root);
    freeOperationTree(tree);
    printf("%s\n", test == true ? "true" : "false");
//    printf("%f\n", d);
    return 0;
}