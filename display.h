#ifndef _DISPLAY_H
#define _DISPLAY_H
#include "list.h"
#include "queue.h"

void* displayloop(void* arg);
void displayInit(Queue* q);
void displayShutdown();

#endif