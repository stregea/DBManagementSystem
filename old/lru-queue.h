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
typedef struct QNode {
    struct QNode *prev, *next;
    int buffer_index; // the index associated with the location of the page within the buffer.
} QNode;
 
// The Queue
typedef struct Queue {
    int count; // Number of filled frames
    int size; // max size of queue
    QNode* front;
    QNode* rear;
} Queue;
 
// A collection of Queue pointer nodes
typedef struct Hash_Struct {
    int capacity; // how many pages can be there
    QNode** array; // an array of queue nodes
} Hash;

// The struct containing the queue and hash
// which will allow for the functionality of using LRU cache.
struct Cache_Struct{
    Hash* hash;
    Queue* queue;
}; typedef struct Cache_Struct* LRU_Cache;
/**
 * Create a node for the queue.
 * @param buffer_index - The id associated with a node.
 * @returns a new node to add to the queue.
 */
QNode* createNode(int buffer_index);

/**
 * Create a Queue that will be used for the LRU nodes.
 * @param size - The max size the queue .
 */
Queue* createQueue(int size);

/**
 * Create a collection (hash) of nodes.
 * @param capacity - The maximum capacity for the hash.
 */
Hash* createHash(int capacity);

/**
 * Create a LRU cache.
 * @param size - The size of the cache to create.
 * @return a cache pointer.
 */
LRU_Cache createCache(int size);

/**
 * Check to see the Queue has enough memory to add a node.
 * @param cache - The cache containing the queue to check.
 * @returns 1 if full, 0 if there is space.
 */
bool queueIsFull(LRU_Cache cache);

/**
 * Check to see if the Queue is empty
 * @param cache - The cache containing the queue to check.
 * @returns 1 if queue is empty, 0 otherwise.
 */
bool queueIsEmpty(LRU_Cache cache);

/**
 * Remove a node from the Queue.
 * @param cache - The cache containing the queue to modify.
 */
void dequeue(LRU_Cache cache);

/**
 * Add a node to the queue.
 * @param cache - The cache containing the queue to modify.
 * @param buffer_index - The buffer_index corresponding to the buffer index containing a page within the storage manager.
 */
void enqueue(LRU_Cache cache, int buffer_index);

/**
 * Call this function whenever a page (page id) is referenced in the driver program.
 * This will make reference of a node and then reorganize the queue.
 * @param cache - The LRU cache containing the queue and hash.
 * @param buffer_index - The index of buffer of the page being referenced.
 */
void referencePage(LRU_Cache cache, int buffer_index);

/**
 * Retrieve the index of the buffer that the LRU page exists within.
 * @param cache - The cache to check.
 * @returns the index to access the LRU page within the buffer.
 */
int getLRUIndexForBuffer(LRU_Cache cache);

/**
 * Free the cache from memory to avoid memory leaks.
 * @param cache - The cache to free.
 */
void freeLRUCache(LRU_Cache cache);
#endif /* lru_queue_h */
