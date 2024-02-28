#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "queue.h"
#define MAX_BUFFER_LENGTH 256

static char* localPort;
static pthread_t recvThread;
static Queue* rd_queue;

void* receiveloop(void* arg) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char buf[MAX_BUFFER_LENGTH];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, localPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "[Receive Thread] getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {
            perror("[Receive Thread] socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("[Receive Thread] bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "[Receiver] failed to bind socket\n");
        return NULL;
    }

    freeaddrinfo(servinfo);

    while (1) {
        addr_len = sizeof their_addr;
        int numbytes = recvfrom(sockfd, buf, MAX_BUFFER_LENGTH - 1, 0, (struct sockaddr*)&their_addr, &addr_len);

        if (numbytes == -1) {
            perror("[Receiver] recvfrom");
            continue;
        }

        buf[numbytes] = '\0';
        char* msg = strdup(buf); // Create message from buffer
        if (msg != NULL) {
            enqueue_msg(rd_queue, msg);
        }
    }

    close(sockfd);
    pthread_exit(NULL);
}

void receiveInit(Queue* q, char* lp) {
    rd_queue = q;
    localPort = lp;

    int x = pthread_create(&recvThread, NULL, receiveloop, NULL);
    if (x != 0) {
        perror("[Receiver] Thread could not be created");
        exit(-1);
    }
}

void receiveShutdown() {
    pthread_cancel(recvThread);
    pthread_join(recvThread, NULL);
}
