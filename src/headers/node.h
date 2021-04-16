//
// Created by sdtre on 4/15/2021.
//

#ifndef DBMANAGEMENTSYSTEM_NODE_H
#define DBMANAGEMENTSYSTEM_NODE_H

struct Node{
    struct Node left;
    int left_child_type;
    struct Node right;
    int right_child_type;
    int operation;
};

struct OperationTree{
    struct Node root;
};


struct Node create_node();

struct OperationTree create_operation_tree();

void freeOperationTree();
#endif //DBMANAGEMENTSYSTEM_NODE_H
