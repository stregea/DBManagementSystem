//
// Created by sdtre on 4/15/2021.
//
#include <stdbool.h>
#include <storagemanager.h>
#ifndef DBMANAGEMENTSYSTEM_NODE_H
#define DBMANAGEMENTSYSTEM_NODE_H


struct Node{
    struct Node * left;
    struct Node * right;
    bool is_operation;
    bool is_conditional;
    int operation;
    int conditional;
    int type;
    char* value;
}; typedef struct Node * Node;

struct OperationTree{
    Node root;
}; typedef struct OperationTree * OperationTree;


Node create_node();

OperationTree create_operation_tree();

void freeOperationTree(OperationTree tree);
#endif //DBMANAGEMENTSYSTEM_NODE_H
