#include "../headers/stack.h"
#include <stdlib.h>

StackData create_stack_data(void *data) {
    StackData stackData = (StackData) malloc(sizeof(struct StackData));

    if (stackData != 0) {
        stackData->data = data;
    }

    return stackData;
}

void free_stack_data(StackData data){
    if(data != NULL){
        free(data);
    }
}

// Utility function to initialize the stack
Stack create_stack() {
    Stack stack = (Stack) malloc(sizeof(struct Stack));
    stack->items = malloc(sizeof(struct StackData));
    stack->size = 0;
    stack->top = -1;
    return stack;
}

// Utility function to return the size of the stack
int size(Stack stack) {
    return stack->size;
}

// Utility function to check if the stack is empty or not
int isEmpty(Stack stack) {
    return stack->top == -1;
}

// Utility function to add an element `x` to the stack
void push(Stack stack, void *data) {
    StackData stackData = create_stack_data(data);
    stack->items = realloc(stack->items, sizeof(StackData) * (size(stack) + 1 ));
    // add an element and increment the top's index
    stack->items[++stack->top] = stackData;
    stack->size++;
}

// Utility function to return the top element of the stack
void *peek(Stack stack) {
    // check for an empty stack
    if (!isEmpty(stack)) {
        return stack->items[stack->top]->data;
    }
    return NULL;
}

// Utility function to pop a top element from the stack
void *pop(Stack stack) {
    // check for stack underflow
    if (!isEmpty(stack)) {
        void* data = stack->items[stack->top]->data;
        free_stack_data(stack->items[stack->top--]);
        return data;
    }
    return NULL;
}

void free_stack(Stack stack){
    for(int i = 0; i < stack->top; i++){
        free_stack_data(stack->items[i]);
    }
    free(stack->items);
    free(stack);
}
