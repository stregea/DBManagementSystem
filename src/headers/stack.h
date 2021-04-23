#ifndef DBMANAGEMENTSYSTEM_STACK_H
#define DBMANAGEMENTSYSTEM_STACK_H
struct StackData {
    void *data;
};
typedef struct StackData *StackData;

// Data structure to represent a stack
struct Stack {
    StackData *items;
    int size;
    int top;
};
typedef struct Stack *Stack;

/**
 * Create data to be inserted within the stack.
 * @param data - The data to insert.
 * @return data to insert into a stack.
 */
StackData create_stack_data(void *data);

/**
 * Initialize a stack.
 * @return a new stack.
 */
Stack create_stack();

/**
 * Get the current size of the stack.
 * @param stack - The stack to determine the size of.
 * @return The size of the stack.
 */
int size(Stack stack);

/**
 * Determine if the stack is empty.
 * @param stack - The stack to check.
 * @return True if empty, false otherwise.
 */
int isEmpty(Stack stack);

/**
 * Push data onto the stack.
 * @param stack - The stack to modify.
 * @param data - The data to insert.
 */
void push(Stack stack, void *data);

/**
 * Return an elements from the stop of the stack.
 * @param stack - The stack to peek.
 * @return The data that lies at the top of the stack.
 */
void *peek(Stack stack);

/**
 * Pop data off of the stack.
 * @param stack - The stack to modify.
 * @return data that existed at the top of the stack.
 */
void *pop(Stack stack);

/**
 * Free the memory of the stack.
 * @param stack - The stack to free.
 */
void free_stack(Stack stack);
#endif //DBMANAGEMENTSYSTEM_STACK_H
