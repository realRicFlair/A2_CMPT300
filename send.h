#ifndef _SEND_H
#define _SEND_H
#include "list.h"
#include "queue.h"

void sendInit(Queue* q, char* LocalPort, char* RemoteName, char* RemotePort);
void sendShutdown();
void sendCancel();

#endif