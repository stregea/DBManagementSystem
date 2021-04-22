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

    int i;
    double d;
//    bool b;
    // check if value can be cast as boolean
    // determine if integer value
    // check if boolean value
    // check if string value.
    // check if a integer value
    if (sscanf(value, "%d", &i) != 0) { // it's an integer
        return INTEGER;
    }
    if (sscanf(value, "%lf", &d) != 0) { // it's an float/double
        return DOUBLE;
    }
    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0) {
        return BOOL;
    }

    return CHAR; // return as a char/varchar.
}

//todo
OperationTree build_tree(char *expression) {
    char *exp_dup = strdup(expression);
    StringArray expression_list = expression_to_string_list(exp_dup);
    StringArray string = infix_to_postfix(expression_list);

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

    tree->root = (Node) peek(stack);
    free_stack(stack);
    free_string_array(string);
    free_string_array(expression_list);
    return tree;
}

// Utility function to return the integer value
// of a given string
int toInt(char *string) {
    return atoi(string);
}

// Utility function to return the boolean value
// of a given string
bool toBool(char *string) {
    return strcasecmp(string, "true") == 0;
}

// Utility function to return the double value
// of a given string
double toDouble(char *value) {
    char *val_pointer;
    return strtod(value, &val_pointer);
}

// Utility function to return the string value
// of a given string
int calculate_string_value(char *value) {
    int ret = 0;
    for (int i = 0; i < strlen(value); i++) {
        ret += value[i];
    }
    return ret;
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
                return toDouble(node->value);
            case CHAR:
            case VARCHAR:
                return calculate_string_value(node->value);
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

bool determine_conditional(Node node) {

    // evaluate left side of tree
    double left_branch = evaluate_tree(node->left);

    // evaluate right side of tree.
    double right_branch = evaluate_tree(node->right);

    // conditional at the top of the tree
    switch (node->conditional) {
        case LESS_THAN:
            return left_branch < right_branch;
        case LESS_THAN_OR_EQUAL_TO:
            return left_branch <= right_branch;
        case GREATER_THAN:
            return left_branch > right_branch;
        case GREATER_THAN_OR_EQUAL_TO:
            return left_branch >= right_branch;
        case EQUALS:
            return left_branch == right_branch;
        case NOT_EQUALS:
            return left_branch != right_branch;
    }

    return false;
}

int precedence(char *operation) {
    //printf("operand: %s\n", operation);
    //printf("operand char: %c\n\n", operation[0]);
    switch (operation[0]) {
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

bool is_character_operator(char character) {
    return character == '+'
           || character == '-'
           || character == '*'
           || character == '/'
           || character == '>'
           || character == '<'
           || character == '='
           || character == '!';
}

StringArray expression_to_string_list(char *expression) {
    //printf("%s\n", expression);
    char *sub_string = NULL;
    int sub_string_length;
    char current_character;

    int length = strlen(expression);
    // definitely a memory issue
    char **tokens = malloc(sizeof(char *) * length);
    int token_count = 0;

    for (int i = 0; i < length; i++) {
        current_character = expression[i];
        //printf("%c\n", current_character);

        if (is_character_operator(current_character)) {
            // does the expression start with an operator
            if (sub_string == NULL) {
                fprintf(stderr, "Error: invalid expression starts with operator\n");
                return NULL;
            }
                // three operands in a row
            else if (is_character_operator(sub_string[0]) && strlen(sub_string) > 1) {
                printf("%s\n", sub_string);
                printf("%d\n", strlen(sub_string));
                fprintf(stderr, "Error: invalid expression\n");
                return NULL;
            }
                // handles two character operands
            else if (is_character_operator(sub_string[0])) {
                sub_string_length = strlen(sub_string);
                sub_string = realloc(sub_string, sub_string_length + 2);
                sub_string[sub_string_length + 1] = 0;
                sub_string[sub_string_length] = current_character;

                if (get_conditional(sub_string) == -1) {
                    fprintf(stderr, "Error: invalid expression\n");
                    return NULL;
                }
            } else {
                tokens[token_count] = strdup(sub_string);
                token_count++;
                sub_string = malloc(sizeof(char));
                strcpy(sub_string, " ");
                sub_string[0] = current_character;
            }
        } else if (current_character != ' ') {
            if (sub_string == NULL) {
                sub_string = malloc(sizeof(char));
                strcpy(sub_string, " ");
                // initial null terminator
                sub_string[0] = 0;
                sub_string[1] = 0;
                sub_string_length = strlen(sub_string);
            } else if (is_character_operator(sub_string[0])) {
                tokens[token_count] = strdup(sub_string);
                token_count++;
                sub_string = malloc(sizeof(char));
                strcpy(sub_string, " ");
                sub_string[0] = 0;
                sub_string[1] = 0;
                sub_string_length = strlen(sub_string);
            } else {
                sub_string_length = strlen(sub_string);
                sub_string = realloc(sub_string, sub_string_length + 2);
                sub_string[sub_string_length + 1] = 0;
            }

            sub_string[sub_string_length] = current_character;

            // add last substring
            if (i == length - 1) {
                tokens[token_count] = strdup(sub_string);
                token_count++;
            }
        }
    }

    if (sub_string != NULL) {
        free(sub_string);
    }
    StringArray strings = malloc(sizeof(struct StringArray));
    strings->array = tokens;
    strings->size = token_count;

    return strings;
}

StringArray infix_to_postfix(StringArray expression) {
    QueueADT queue = que_create();
    Stack stack = create_stack();

    for (int i = 0; i < expression->size; i++) {
        // all even indices are numerical values
        if (i % 2 == 0) {
            //printf("number: %s\n", expression->array[i]);
            que_insert(queue, expression->array[i]);
        }
            // if the string is an operand
        else {
            while (!isEmpty(stack) && precedence(expression->array[i]) <= precedence((char *) peek(stack))) {
                // pop operators from the operator stack onto the output queue
                que_insert(queue, (char *) pop(stack));
            }
            // push it onto the operator stack
            push(stack, expression->array[i]);
        }
    }

    while (!isEmpty(stack)) {
        que_insert(queue, (char *) pop(stack));
    }

    StringArray postfix = malloc(sizeof(struct StringArray));
    postfix->size = (int) sizeQue(queue);
    postfix->array = malloc(postfix->size * sizeof(char *));

    char *current_string;
    for (int i = 0; i < postfix->size; i++) {
        current_string = (char *) que_remove(queue);
        postfix->array[i] = malloc(strlen(current_string) + 1);
        strcpy(postfix->array[i], current_string);
    }

    que_destroy(queue);
    free_stack(stack);
    return postfix;
}
