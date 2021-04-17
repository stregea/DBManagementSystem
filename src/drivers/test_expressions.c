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


void printNodes(Node node) {
    if (node != NULL) {
        printNodes(node->left);
//        if(node->left != NULL && node->right != NULL){
        if (node->is_operation) {
            switch (node->operation) {
                case ADDITION:
                    printf("+\n");
                    break;
                case SUBTRACTION:
                    printf("-\n");
                    break;
                case DIVISION:
                    printf("/\n");
                    break;
                case MULTIPLICATION:
                    printf("*\n");
                    break;
            }
            if(node->right->value != NULL && node->left->value != NULL){
                printf("%s %s\n", node->right->value, node->left->value);
            }
        } else if (node->is_conditional) {
            // todo
        }
        printNodes(node->right);
    }
}

int main() {
    char *expression = "21*34+1/2";
    char *string = malloc(strlen(expression) + 1);
    strcpy(string, expression);
    StringArray strings = expression_to_string_list(string);
    strings = infix_to_postfix(strings);

    for (int i = 0; i < strings->size; i++) {
        printf("%s ", strings->array[i]);
    }
    printf("\n");
    OperationTree tree = build_tree(strings);

    printNodes(tree->root);
//    printf("%d\n", tree->root->operation);
    return 0;
}