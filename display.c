#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "queue.h"

static Queue* rd_queue;
static pthread_t displayThread;

static char* msg;

void* displayloop(void* arg) {
    while (1) {
        msg = dequeue_msg(rd_queue);
        if (msg != NULL) {
            printf("Received: %s\n", msg);
            free(msg);
        }
    }

    pthread_exit(NULL);
}

void displayInit(Queue* q) {
    rd_queue = q;

    int x = pthread_create(&displayThread, NULL, displayloop, NULL);
    if (x != 0) {
        perror("[Display] Thread could not be created");
        exit(-1);
    }
}

void displayCancel() { pthread_cancel(displayThread); }

void displayShutdown() {
    pthread_cancel(displayThread);
    pthread_join(displayThread, NULL);
    msg = NULL;
    printf("[Display Thread] Shutdown\n");
}
