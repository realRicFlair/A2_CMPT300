#ifndef _RECEIVE_H
#define _RECEIVE_H
#include "list.h"
#include "queue.h"

void* receiveloop(void* arg);
void receiveInit(Queue* rd_queue, char* lp);
void receiveShutdown();

#endif