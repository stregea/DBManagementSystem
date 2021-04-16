#include "../headers/node.h"
#include "../headers/Enums.h"
#include <stdlib.h>


struct Node * create_node(){
    struct Node * node = malloc(sizeof(struct Node));
    node->left = NULL;
    node->right = NULL;
    node->is_operation = false;
    node->is_conditional = false;
    node->value = NULL;
    node->operation = INVALID;
    node->conditional = INVALID;
    node->type = INVALID;
    return node;
} typedef struct Node * Node;


OperationTree create_operation_tree(){
    OperationTree tree = NULL;
    return tree;
}

void free_nodes(Node root){
    if(root != NULL){
        free_nodes(root->left);
        free_nodes(root->right);
        if(root->value != NULL) {
            free(root->value);
        }
        free(root);
    }
}

void freeOperationTree(OperationTree tree){
    if(tree != NULL){
        free_nodes(tree->root);
        free(tree);
    }
}