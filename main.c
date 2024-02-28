#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "keyboard.h"
#include "list.h"
#include "queue.h"
#include "receive.h"
#include "send.h"

// Launches
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Invalid Arguments!\n Should look like this: ");
        printf(
            "./s-talk [local port] "
            "[remote machine name]"
            "[remote port number]\n");
        return -1;
    }

    char* localPort = argv[1];
    char* remoteName = argv[2];
    char* remotePort = argv[3];

    Queue* ks_queue = Queue_Create();  // Keyboard + Send Queue
    Queue* rd_queue = Queue_Create();  // Receive + Display Queue

    if (ks_queue == NULL || rd_queue == NULL) {
        printf("Failed to create List \n");
        return 1;
    }

    // Thread Startup
    keyboardInit(ks_queue);
    sendInit(ks_queue, localPort, remoteName, remotePort);
    receiveInit(rd_queue, localPort);
    displayInit(rd_queue);

    // Wait for Threads to finish
    keyboardShutdown();
    sendShutdown();
    receiveShutdown();
    displayShutdown();

    // Cleanup
    printf("Exiting\n");
    Queue_Destroy(ks_queue);
    Queue_Destroy(rd_queue);
    return 0;
}
