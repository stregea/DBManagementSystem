#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include "../headers/shunting_yard_parser.h"
#include "../headers/queue.h"
#include "../headers/stack.h"
#include "../headers/Enums.h"

bool is_operator(char operator) {
    int operation = get_operation(operator);
    return operation == ADDITION || operation == SUBTRACTION || operation == DIVISION || operation == MULTIPLICATION;
}

bool is_conditional(char *condition) {
    int conditional = get_conditional(condition);
    return conditional == EQUALS || conditional == GREATER_THAN || conditional == GREATER_THAN_OR_EQUAL_TO ||
           conditional == LESS_THAN || conditional == LESS_THAN_OR_EQUAL_TO || conditional == NOT_EQUALS;
}

int get_data_type(char *value) {

    // check if value can be cast as boolean
    // determine if integer value
    // check if boolean value
    // check if string value.

    return INTEGER;
}

//todo
OperationTree build_tree(StringArray string) {
    OperationTree tree = create_operation_tree();
    Stack stack = create_stack();
    Node node = NULL;
    Node left_child = NULL;
    Node right_child = NULL;

    for (int i = 0; i < string->size; i++) {
        if (!is_operator(*string->array[i]) && !is_conditional(string->array[i])) {
            node = create_node();
            // determine node type
            node->type = get_data_type(string->array[i]); //todo
            node->value = strdup(string->array[i]);
            push(stack, node);
        } else {
            node = create_node();
            if (is_operator(*string->array[i])) {
                node->is_operation = true;
                node->operation = get_operation(*string->array[i]);
            } else {
                node->is_conditional = true;
                node->conditional = get_conditional(string->array[i]);
            }
            node->value = strdup(string->array[i]);

            right_child = (Node) pop(stack);
            left_child = (Node) pop(stack);

            node->left = left_child;
            node->right = right_child;

            push(stack, node);
        }
    }

    tree->root = (Node)peek(stack);
    free_stack(stack);
    return tree;
}

// Utility function to return the integer value
// of a given string
int toInt(char *string) {
    return atoi(string);
}

bool toBool(char* string){
    return strcasecmp(string, "true") == 0;
}

double toDouble(char *value) {
    char *val_pointer;
    return strtod(value, &val_pointer);
}

double evaluate_tree(Node node) {

    if (!node) {
        return 0;
    }

    // if a leaf node
    if (!node->left && !node->right) {

        // determine type of value (int, double, bool, char, varchar)
        switch (node->type) {
            case INTEGER: // or bool
                 return toInt(node->value);
            case BOOL:
                return toBool(node->value);
            case DOUBLE:
            case CHAR:
            case VARCHAR:
                return toDouble(node->value);
        }
    }

    double left_value = evaluate_tree(node->left);
    double right_value = evaluate_tree(node->right);

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
    return DBL_MAX;
}

bool determine_conditional(Node node){

    // evaluate left side of tree
    double left_value = evaluate_tree(node->left);

    // evaluate right side of tree.
    double right_value = evaluate_tree(node->right);

    // conditional at the top of the tree
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

    return false;
}

int precedence(char *operation) {
    //printf("operand: %s\n", operation);
    //printf("operand char: %c\n\n", operation[0]);
    switch(operation[0]) {
        case '+' :
            return 2;
        case '-' :
            return 2;
        case '*' :
            return 3;
        case '/' :
            return 3;
        case '^' :
            return 4;
        default :
            //printf("Invalid operand\n" );
            return -1;
   }
}

StringArray expression_to_string_list(char *expression) {
    
    char* tmp = strdup(expression);
    int length = strlen(expression);
    // will have to fix this allocation from freeing issues
    char** tokens = malloc(length * sizeof(char *));
    int token_index = 0;

    char * ops = "*-+/";
    char *token = strtok(tmp, ops);

    // add all numerical values to string array
    while(token != NULL) {
        // store number token
        tokens[token_index] = malloc(strlen(token) + 1);
        strcpy(tokens[token_index], token);
        token_index++;
        
        // allocate space for operation char
        tokens[token_index] = malloc(sizeof(char) + 1);
        strcpy(tokens[token_index], " ");
        token_index++;

        // get next token
        token = strtok(NULL, ops);
    }

    // add all op strings
    int ops_index = 1;
    for(int i = 0; i < length; i++) {
        if(expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            tokens[ops_index][0] = expression[i];
            ops_index += 2;
        }
    }

    StringArray strings = malloc(sizeof(struct StringArray));
    strings->array = tokens;
    strings->size = token_index - 1;

    return strings;
}

StringArray infix_to_postfix(StringArray expression) {
    QueueADT queue = que_create();
    Stack stack = create_stack();

    for(int i = 0; i < expression->size; i++) {
        // all even indices are numerical values
        if(i % 2 == 0) {
            //printf("number: %s\n", expression->array[i]);
            que_insert(queue, expression->array[i]);
        }
        // if the string is an operand
        else{
            while(!isEmpty(stack) && precedence(expression->array[i]) <= precedence((char*)peek(stack))) {
                // pop operators from the operator stack onto the output queue
                que_insert(queue, (char*)pop(stack));
            }
            // push it onto the operator stack
            printf("operand: %s\n", expression->array[i]);
            push(stack, expression->array[i]);
            printf("operand stack: %s\n", (char*)peek(stack));
        }
    }

    while(!isEmpty(stack)) {
        que_insert(queue, (char*)pop(stack));
    }

    StringArray postfix = malloc(sizeof(struct StringArray));
    postfix->size = (int)sizeQue(queue);
    postfix->array = malloc(postfix->size * sizeof(char*));

    char *current_string;
    for(int i = 0; i < postfix->size; i++) {
        current_string = (char*)que_remove(queue);
        postfix->array[i] = malloc(strlen(current_string) + 1);
        strcpy(postfix->array[i], current_string);
    }

    que_destroy(queue);
    free_stack(stack);
    return postfix;
}
