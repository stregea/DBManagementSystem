/// received from Scott Johnson, Spring 2019
/// QueueADT class file.

# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <stdbool.h>
# include <assert.h>

typedef struct queueData {
    void * data;
    struct queueData * next;
} QueueData;

struct queueStruct {
    QueueData *front, *back;
    size_t size;
};

typedef struct queueStruct *QueueADT;

#define _QUEUE_IMPL_
#include "../headers/queue.h"

static QueueData *create_queue_data( void * data ){
    QueueData *new;

    new = (QueueData *) malloc( sizeof(QueueData));
    if (new != 0){
        new->data = data;
        new->next = 0;
    }

    return new;
}


QueueADT que_create( ){
    QueueADT new;

    new = (QueueADT) malloc(sizeof(struct queueStruct));

    if (new != 0){
        new->front = new->back = 0;
        new->size = 0;
    }

    return new;
}

void que_destroy( QueueADT queue ){
    QueueData *tmp;

    while( queue->front != 0){
        tmp = queue->front;
        queue->front = queue->front->next;
        free(tmp);
    }

    free(queue);
}


void que_clear( QueueADT queue ){
    QueueData *tmp;

    while( queue->front != 0){
        tmp = queue->front;
        queue->front = queue->front->next;
        free(tmp);
    }

    queue->front = queue->back = 0;
    queue->size = 0;
}

void * front( QueueADT queue){
    return queue->front->data;
}



void * back( QueueADT queue){
    return queue->back->data;
}

void que_insert( QueueADT queue, void * data ){
    QueueData *new = create_queue_data(data);

    if(queue->front == 0){
        queue->front = queue->back = new;
    }
    else{
        queue->back->next = new;
        queue->back = new;
    }

    queue->size++;
}

void * que_remove( QueueADT queue ){
    void *value = queue->front->data;
    QueueData *tmp = queue->front;

    queue->front = queue->front->next;
    queue->size--;

    if(queue->front == 0){
        queue->back = 0;
    }

    free(tmp);
    return value;
}

size_t sizeQue(QueueADT queue){
    return queue->size;
}

bool que_empty(QueueADT queue){
    return queue->size == 0;
}