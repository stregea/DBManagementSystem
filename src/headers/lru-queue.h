/**
 * CSCI-421 Project: Phase1
 * @file lru-queue.h
 * Description:
 *         Header file for ../c/lru-queue.c. This will allow for the implementation of a Least Recently Used cache.
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */

#ifndef lru_queue_h
#define lru_queue_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// A Node for the Queue
// This will represent a page at a certain index within the buffer
struct QNode {
    struct QNode *prev, *next;
    int buffer_index; // the index associated with the location of the page within the buffer.
}; typedef struct QNode* QNode;
 
// The Queue
struct Queue {
    int count; // Number of filled frames
    int size; // max size of queue
    QNode front;
    QNode rear;
};typedef struct Queue* Queue;
 
// A collection of Queue pointer nodes
struct QueueHash {
    int capacity; // how many pages can be there
    QNode* array; // an array of queue nodes
};typedef struct QueueHash* Hash;

/**
 * Create a node for the queue.
 * @param buffer_index - The id associated with a node.
 * @returns a new node to add to the queue.
 */
QNode createNode(int buffer_index);

/**
 * Create a Queue that will be used for the LRU nodes.
 * @param size - The max size the queue .
 */
Queue createQueue(int size);

/**
 * Create a collection (hash) of nodes.
 * @param capacity - The maximum capacity for the hash.
 */
Hash createHash(int capacity);

/**
 * Check to see the Queue has enough memory to add a node.
 * @param queue - The queue to check.
 * @returns 1 if full, 0 if there is space.
 */
bool queueIsFull(Queue queue);

/**
 * Check to see if the Queue is empty
 * @param queue - the queue to check
 * @returns 1 if queue is empty, 0 otherwise.
 */
bool queueIsEmpty(Queue queue);

/**
 * Remove a node from the Queue.
 * @param queue - The Queue to modify.
 */
void dequeue(Queue queue);

/**
 * Add a node to the queue.
 * @param queue - The Queue to modify.
 */
void enqueue(Queue queue, Hash hash, int buffer_index);

/**
 * Call this function whenever a page (page id) is referenced in the driver program.
 * This will make reference of a node and then reorganize the queue.
 * @param queue - The Queue to modify
 * @param hash - A collection of queue nodes.
 * @param buffer_index - The index of buffer of the page being referenced.
 */
void referencePage(Queue queue, Hash hash, int buffer_index);

/**
 * Free the cache from memory to avoid memory leaks.
 * @param queue - The Queue to free from memory.
 * @param hash - The Hash to free from memory.
 */
void freeLRUCache(Queue queue, Hash hash);
#endif /* lru_queue_h */
