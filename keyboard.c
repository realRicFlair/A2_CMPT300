#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "queue.h"

#define MAX_BUFFER_LENGTH 256

//Runs keyboard thread
static pthread_t kthread;
static Queue* ks_queue;

static void* runloop(void* arg) {
    char* buffer = NULL;
    size_t bufferlen = 0;
    ssize_t msg_len;
    
    while ((msg_len = getline(&buffer, &bufferlen, stdin)) != -1) {
        if (buffer[msg_len - 1] == '\n') {
            buffer[msg_len - 1] = '\0';
            msg_len--;
        }

        // Break when exit command
        if (strcmp(buffer, "!exit") == 0 || strcmp(buffer, "!") == 0) {
            break;
        }

        char* msg = (char*)malloc(msg_len + 1);
        if (msg == NULL) {
            perror("[Keyboard Thread] Failed to allocate mem for message");
        }
        strcpy(msg, buffer);
        int enqueue_result = enqueue_msg(ks_queue, msg);
        
        if (enqueue_result == -1){
            perror("[Keyboard Thread] Queue full/error. Message possibly too long\n");
        }

    }
    printf("Exiting . . .\n");
    free(buffer);
    pthread_exit(NULL);
}

void keyboardInit(Queue* q) {
    ks_queue = q;

    int x = pthread_create(&kthread, NULL, runloop, NULL); 
    if (x != 0) {
        perror("[Keyboard Thread] thread could not be created");
        exit(-1);
    }
}

void keyboardCancel() { pthread_cancel(kthread); }
void keyboardShutdown() {
    pthread_join(kthread, NULL);
    printf("[Keyboard Thread] Shutdown\n");
}