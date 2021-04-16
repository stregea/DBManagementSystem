#include "../headers/node.h"
#include <stdlib.h>


struct Node create_node(){
    struct Node node = {
            .left = {},
            .right = {},
            .left_child_type = -1,
            .right_child_type = -1,
            .operation = -1,
    };
    return node;
}
//
//void insert_left_child(struct OperationTree tree, struct Node node){
//
//    if(node == NULL){
//        node = create_node();
//    }
//
//}

struct OperationTree create_operation_tree(){
    struct OperationTree tree = {
           .root = create_node(),
    };
    return tree;
}

//void freeOperationTree(){
//
//}