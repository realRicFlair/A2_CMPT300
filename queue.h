// Queue system. Uses List to implement a queue, while retaining synchronization
#ifndef _QUEUE_H
#define _QUEUE_H
#include <pthread.h>

#include "list.h"

typedef struct Queue {
    List* list;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

Queue* Queue_Create();
void Queue_Destroy(Queue* queue);
int enqueue_msg(Queue* queue, char* msg);
char* dequeue_msg(Queue* queue);
int count_msg(Queue* queue);

#endif