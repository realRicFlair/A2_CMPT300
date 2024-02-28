// Queue system. Uses List to implement a queue, while retaining synchronization
#include "queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

Queue* Queue_Create() {
    Queue* q = malloc(sizeof(Queue));
    if (!q) return NULL;

    q->list = List_create();
    if (!q->list) {
        free(q);
        return NULL;
    }

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);

    return q;
}

void Queue_Destroy(Queue* q) {
    if (q) {
        pthread_mutex_destroy(&q->mutex);
        pthread_cond_destroy(&q->cond);

        List_free(q->list, free);
        free(q);
    }
}

int enqueue_msg(Queue* q, char* msg) {
    pthread_mutex_lock(&q->mutex);
    int result = List_prepend(q->list, msg);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);

    return result;
}

char* dequeue_msg(Queue* q) {
    pthread_mutex_lock(&q->mutex);
    while (List_count(q->list) == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    char* msg = List_trim(q->list);
    pthread_mutex_unlock(&q->mutex);

    return msg;
}

int count_msg(Queue* q) {
    pthread_mutex_lock(&q->mutex);
    int count = List_count(q->list);
    pthread_mutex_unlock(&q->mutex);

    return count;
}
