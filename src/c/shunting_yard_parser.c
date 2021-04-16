#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include "../headers/shunting_yard_parser.h"
#include "../headers/queue.h"
#include "../headers/stack.h"
#include "../headers/Enums.h"

bool is_operator(char operator) {
    int operation = get_operation(operator);
    return operation == ADDITION || operator == SUBTRACTION || operator == DIVISION || operator == MULTIPLICATION;
}

bool is_conditional(char *condition) {
    int conditional = get_conditional(condition);
    return conditional == EQUALS || conditional == GREATER_THAN || conditional == GREATER_THAN_OR_EQUAL_TO ||
           conditional == LESS_THAN || conditional == LESS_THAN_OR_EQUAL_TO || conditional == NOT_EQUALS;
}

int get_type(char *value) {

    // check if value can be cast as boolean
    // determine if integer value
    // check if boolean value
    // check if string value.

    return INTEGER;
}

// todo implement stack functionality
OperationTree build_tree(StringArray string) {
    OperationTree tree = malloc(sizeof(struct OperationTree));
    //Stack stack = create_stack();
    Node node = NULL;
    Node left_child = NULL;
    Node right_child = NULL;

    for (int i = 0; i < string->size; i++) {
        if (!is_operator(*string->array[i]) && !is_conditional(string->array[i])) {
            node = create_node();
            // determine node type
            node->type = get_type(string->array[i]);
            node->value = strdup(string->array[i]);
            //stack.push(node);
        } else {
            node = create_node();
            if (is_operator(*string->array[i])) {
                node->is_operation = true;
                node->operation = get_operation(*string->array[i]);
            } else {
                node->is_conditional = true;
                node->conditional = get_conditional(string->array[i]);
            }

            //left_child = stack.pop()
            //right_child = stack.pop()

            node->left = left_child;
            node->right = left_child;
            //stack.push(node);
        }
    }

//    tree->root = stack.peek();
// freeStack(stack)
    return tree;
}

double toDouble(char *value) {
    return 0;
}

double evaluate_node_operation(Node node) {

    if (!node) {
        return 0;
    }

    // if a leaf node
    if (!node->left && !node->right) {

        // determine type of value (int, double, bool, char, varchar)
        switch (node->type) {
            case INTEGER: // or bool
            case BOOL:
                // return toInt(value)
                return 0;
            case DOUBLE:
            case CHAR:
            case VARCHAR:
                return toDouble(node->value);
        }
    }

    double left_value = evaluate_node_operation(node->left);
    double right_value = evaluate_node_operation(node->right);

    if (node->is_operation) {
        switch (node->operation) {
            case ADDITION:
                return left_value + right_value;
            case SUBTRACTION:
                return left_value - right_value;
            case DIVISION:
                return left_value / right_value;
            case MULTIPLICATION:
                return left_value * right_value;
        }
    }

    // conditional
    switch (node->operation) {
        case LESS_THAN:
            return left_value < right_value;
        case LESS_THAN_OR_EQUAL_TO:
            return left_value <= right_value;
        case GREATER_THAN:
            return left_value > right_value;
        case GREATER_THAN_OR_EQUAL_TO:
            return left_value >= right_value;
        case EQUALS:
            return left_value == right_value;
        case NOT_EQUALS:
            return left_value != right_value;
    }

    return DBL_MAX;
}
