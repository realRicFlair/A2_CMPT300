#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "list.h"
#include "queue.h"

void keyboardInit(Queue* q);
void keyboardShutdown();
void keyboardCancel();

#endif