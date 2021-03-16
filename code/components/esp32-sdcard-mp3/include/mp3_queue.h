#ifndef MP3_Q
#define MP3_Q

#include <stdio.h>
#include <stdlib.h>

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        char **elements;
} Queue;

Queue * createQueue(int);
void dequeue(Queue*);
void enqueue(Queue*, char*);
char* front(Queue*);

#endif  // MP3_Q