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
QNode createNode(int buffer_index){
    // Allocate memory for the node
    QNode node = malloc(sizeof(struct QNode));
    
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
Queue createQueue(int size){
    // Allocate memory for the queue
    Queue queue = malloc(sizeof(struct Queue));
    
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
Hash createHash(int capacity){
    // Allocate memory for the hash
    Hash hash = malloc(sizeof(struct QueueHash));
    
    hash->capacity = capacity;
    
    hash->array = malloc(hash->capacity * sizeof(QNode));
    
    for(int i = 0; i < hash->capacity; i++){
        hash->array[i] = NULL;
    }
    
    return hash;
}

/**
 * Check to see the Queue has enough memory to add a node.
 * @param queue - The queue to check.
 * @returns 1 if full, 0 if there is space.
 */
bool queueIsFull(Queue queue){
    return queue->count == queue->size;
}

/**
 * Check to see if the Queue is empty
 * @param queue - the queue to check
 * @returns 1 if queue is empty, 0 otherwise.
 */
bool queueIsEmpty(Queue queue){
    return queue->rear == NULL;
}

/**
 * Remove a node from the Queue.
 * @param queue - The Queue to modify.
 */
void dequeue(Queue queue){
    
    if(queueIsEmpty(queue)){
        return;
    }
    
    // if the node being removed is the only one in the list,
    // change the front pointer.
    if(queue->front == queue->rear){
        queue->front = NULL;
    }
    
    // Update the rear pointer
    QNode node = queue->rear;
    queue->rear = queue->rear->prev;
    
    if(queue->rear){
        queue->rear->next = NULL;
    }
    
    // remove the node from memory
    free(node);
    
    // update the queue count
    queue->count--;
}

/**
 * Add a node to the queue.
 * @param queue - The Queue to modify.
 */
void enqueue(Queue queue, Hash hash, int buffer_index){
    if(queueIsFull(queue)){
        // remove page from hash
        // get index of 
        hash->array[queue->rear->buffer_index] = NULL;
        dequeue(queue);
    }
    
    // Create a new node with a given page id.
    // Then add the node to the front of the queue
    QNode node = createNode(buffer_index);
    node->next = queue->front;
    
    if(queueIsEmpty(queue)){
        // change the front and rear pointers
        queue->front = node;
        queue->rear = node;
    }else{
        // change the front
        queue->front->prev = node;
        queue->front = node;
    }
    
    hash->array[buffer_index] = node;
 
    // increment queue count
    queue->count++;
}

/**
 * Call this function whenever a page from a buffer index is referenced.
 * This will make reference of a node and then reorganize the queue.
 * @param queue - The Queue to modify
 * @param hash - A collection of queue nodes.
 * @param buffer_index - The index of buffer of the page being referenced.
 */
void referencePage(Queue queue, Hash hash, int buffer_index){
    QNode requested_page = hash->array[buffer_index];
    
    // if the page is not within the cache, insert it.
    if(requested_page == NULL){
        enqueue(queue, hash, buffer_index);
    }
    // page exists but not at front of queue
    else if(requested_page != queue->front){
        // unlink the requested_page from its current location in the queue
        requested_page->prev->next = requested_page->next;
        
        if(requested_page->next){
            requested_page->next->prev = requested_page->prev;
        }
        
        // if the requested page is at the rear, move the rear node to front of queue.
        if(requested_page == queue->rear){
            queue->rear = requested_page->prev;
            queue->rear->next = NULL;
        }
        
        // Place the requested page before the current front
        requested_page->next = queue->front;
        requested_page->prev = NULL;
        
        // Change previous of current front
        requested_page->next->prev = requested_page;
        
        // Chnage the front of the queue to the requested page.
        queue->front = requested_page;
    }
}

/**
 * Free the cache from memory to avoid memory leaks.
 * @param queue - The Queue to free from memory.
 * @param hash - The Hash to free from memory.
 */
void freeLRUCache(Queue queue, Hash hash){
   
    // iterate through the hash and free the QNodes.
    for(int i = 0; i<hash->capacity; i++){
        if(hash->array[i] != NULL){
            free(hash->array[i]);
        }
    }
    
    // I'm not sure this is necessary since nodes are being freed above,
    // but this will need to be debugged and checked eventually.
    // iterate through the queue and free the nodes
//    QNode tmp;
//    QNode cursor = queue->front;
//    while(cursor != NULL){
//        tmp = cursor;
//        cursor = cursor->next;
//        free(tmp);
//    }
    
}
