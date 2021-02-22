/**
 * CSCI-421 Project: Phase1
 * @file lru-queue.h
 * Description:
 *         This file contains the defintions for ../h/lru-queue.h. This will allow for the implementation of a Least Recently Used cache.
 *
 * @author Kyle Collins  (kxc1981@rit.edu)
 * @author Geoffrey Moss (gbm2613@rit.edu)
 * @author Sam Tillinghast  (sft6463@rit.edu)
 * @author Samuel Tregea  (sdt1093@rit.edu)
 */

#include "../headers/lru-queue.h"

/**
 * Create a node for the queue.
 * @param buffer_index - The index associated with a node that corresponds to the location in the hash and buffer.
 * @returns a new node to add to the queue.
 */
QNode* createNode(int buffer_index){
    // Allocate memory for the node
    QNode* node = malloc(sizeof(struct QNode));
    
    // Assign the node to the corresponding page id.
    node->buffer_index = buffer_index;
    
    // set the pointers to NULL
    node->next = NULL;
    node->prev = NULL;
    
    return node;
}

/**
 * Create a Queue that will be used for the LRU nodes.
 * @param size - The max size the queue .
 */
Queue* createQueue(int size){
    // Allocate memory for the queue
    Queue* queue = malloc(sizeof(struct Queue));
    
    // initialize the queue
    queue->size = size;
    queue->count = 0;
    
    // handle pointers
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

/**
 * Create a collection (hash) of nodes.
 * @param capacity - The maximum capacity for the hash.
 */
Hash* createHash(int capacity){
    // Allocate memory for the hash
    Hash* hash = malloc(sizeof(Hash));
    
    hash->capacity = capacity;
    
    hash->array = malloc(hash->capacity * sizeof(QNode));
    
    for(int i = 0; i < hash->capacity; i++){
        hash->array[i] = NULL;
    }
    
    return hash;
}

/**
 * Create a LRU cache.
 * @param size - The size of the cache to create.
 * @return a cache pointer.
 */
LRU_Cache createCache(int size){
    LRU_Cache cache = malloc(sizeof(struct Cache_Struct));
    cache->queue = createQueue(size);
    cache->hash = createHash(size);
    return cache;
}

/**
 * Check to see the Queue has enough memory to add a node.
 * @param cache - The cache containing the queue to check.
 * @returns 1 if full, 0 if there is space.
 */
bool queueIsFull(LRU_Cache cache){
    return cache->queue->count == cache->queue->size;
}

/**
 * Check to see if the Queue is empty
 * @param cache - The cache containing the queue to check.
 * @returns 1 if queue is empty, 0 otherwise.
 */
bool queueIsEmpty(LRU_Cache cache){
    return cache->queue->rear == NULL;
}

/**
 * Remove a node from the Queue.
 * @param cache - The cache containing the queue to modify.
 */
void dequeue(LRU_Cache cache){
    
    if(queueIsEmpty(cache)){
        return;
    }
    
    // if the node being removed is the only one in the list,
    // change the front pointer.
    if(cache->queue->front == cache->queue->rear){
        cache->queue->front = NULL;
    }
    
    // Update the rear pointer
    QNode* node = cache->queue->rear;
    cache->queue->rear = cache->queue->rear->prev;
    
    if(cache->queue->rear){
        cache->queue->rear->next = NULL;
    }
    
    // remove the node from memory
    free(node);
    
    // update the queue count
    cache->queue->count--;
}

/**
 * Add a node to the queue.
 * @param cache - The cache containing the queue to modify.
 * @param buffer_index - The buffer_index corresponding to the buffer index containing a page within the storage manager.
 */
void enqueue(LRU_Cache cache, int buffer_index){
    if(queueIsFull(cache)){
        // remove page from hash
        // get index of 
        cache->hash->array[cache->queue->rear->buffer_index] = NULL;
        dequeue(cache);
    }
    
    // Create a new node with a given page id.
    // Then add the node to the front of the queue
    QNode* node = createNode(buffer_index);
    node->next = cache->queue->front;
    
    if(queueIsEmpty(cache)){
        // change the front and rear pointers
        cache->queue->front = node;
        cache->queue->rear = node;
    }else{
        // change the front
        cache->queue->front->prev = node;
        cache->queue->front = node;
    }
    
    cache->hash->array[buffer_index] = node;
 
    // increment queue count
    cache->queue->count++;
}

/**
 * Call this function whenever a page (page id) is referenced in the driver program.
 * This will make reference of a node and then reorganize the queue.
 * @param cache - The LRU cache containing the queue and hash.
 * @param buffer_index - The index of buffer of the page being referenced.
 */
void referencePage(LRU_Cache cache, int buffer_index){

    if (buffer_index == -1) {
        // page could not be found, cannot be added to buffer or LRU
        return;
    }

    printf("referencing page at index %d\n", buffer_index);
    QNode* requested_page = cache->hash->array[buffer_index];
    
    // if the page is not within the cache, insert it.
    if(requested_page == NULL){
        enqueue(cache, buffer_index);
    }
    // page exists but not at front of queue
    else if(requested_page != cache->queue->front){
        // unlink the requested_page from its current location in the queue
        requested_page->prev->next = requested_page->next;
        
        if(requested_page->next){
            requested_page->next->prev = requested_page->prev;
        }
        
        // if the requested page is at the rear, move the rear node to front of queue.
        if(requested_page == cache->queue->rear){
            cache->queue->rear = requested_page->prev;
            cache->queue->rear->next = NULL;
        }
        
        // Place the requested page before the current front
        requested_page->next = cache->queue->front;
        requested_page->prev = NULL;
        
        // Change previous of current front
        requested_page->next->prev = requested_page;
        
        // Chnage the front of the queue to the requested page.
        cache->queue->front = requested_page;
    }
}

/**
 * Retrieve the index of the buffer that the LRU page exists within.
 * @param cache - The cache to check.
 * @returns the index to access the page within the buffer.
 */
int getLRUIndexForBuffer(LRU_Cache cache){
    return cache->queue->front->buffer_index;
}

/**
 * Free the cache from memory to avoid memory leaks.
 * @param cache - The cache to free.
 */
void freeLRUCache(LRU_Cache cache){
   
    // iterate through the hash and free the QNodes.
    for(int i = 0; i<cache->hash->capacity; i++){
        if(cache->hash->array[i] != NULL){
            free(cache->hash->array[i]);
        }
    }

    free(cache->hash->array);
    free(cache->hash);
    // I'm not sure this is necessary since nodes are being freed above,
    // but this will need to be debugged and checked eventually.
    // iterate through the queue and free the nodes
//    QNode tmp;
//    QNode cursor = cache->queue->front;
//    while(cursor != NULL){
//        tmp = cursor;
//        cursor = cursor->next;
//        free(tmp);
//    }
    free(cache->queue);
    free(cache);
}
