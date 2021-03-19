#ifndef MP3_Q
#define MP3_Q

#include <stdio.h>
#include <stdlib.h>

typedef struct queue
{
        char *data;
        struct queue *next;
} Queue;

void enqueue(Queue**, char*);
char* front(Queue**);
void freeQueue(Queue**);

#endif  // MP3_Q