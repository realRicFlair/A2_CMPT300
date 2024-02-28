// Heavily inspired by Beej's guide to network programming examples
#include "send.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"
#include "queue.h"

static char* remoteName;
static char* remotePort;
static char* localPort;
static Queue* ks_queue;
static pthread_t sendThread;
static int shouldTerminate = false;
static struct addrinfo hints, *servinfo, *p;
static int sockfd;

void* sendloop(void* arg) {
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    // Set to AF_INET to force IPv4, AF_INET6 for IPv6, or AF_UNSPEC for any
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(remoteName, remotePort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "[Sender] getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    // Loop through getaddr linked list, and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("[Sender] Socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "[Sender] Failed to create socket\n");
        return NULL;
    }

    while (!shouldTerminate) {
        char* msg = dequeue_msg(ks_queue);
        if (msg != NULL) {
            if ((numbytes = sendto(sockfd, msg, strlen(msg), 0, p->ai_addr, p->ai_addrlen)) == -1) {
                perror("[Sender] sendto");
                continue;
            }

            //for debugging purposes mostly
            //printf("[Send Thread] Sent: %s\n", msg);
            free(msg);
        }
    }
    printf("Freeing");
    freeaddrinfo(servinfo);
    close(sockfd);
    pthread_exit(NULL);
}

void sendInit(Queue* q, char* lp, char* rmnm, char* rmprt) {
    ks_queue = q;
    localPort = lp;
    remoteName = rmnm;
    remotePort = rmprt;

    int x = pthread_create(&sendThread, NULL, sendloop, NULL);
    if (x != 0) {
        perror("[Send Thread] Thread could not be created");
        exit(-1);
    }
}

void sendCancel() { 
    
    pthread_cancel(sendThread); 
}
void sendShutdown() {
    shouldTerminate = true;
    printf("Freeing");
    freeaddrinfo(servinfo);
    close(sockfd);

    pthread_cancel(sendThread);
    pthread_join(sendThread, NULL);
    printf("[Send Thread] Shutdown\n");
}