#include "mp3_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

Queue * createQueue(int maxElements)
{
        //Create a Queue
        Queue *q;
        q = (Queue *)malloc(sizeof(Queue));
        //Initialise its properties
        q->elements = (char**)malloc(sizeof(char*)*maxElements);
        q->size = 0;
        q->capacity = maxElements;
        q->front = 0;
        q->rear = -1;
        //Return the pointer
        return q;
}

void dequeue(Queue *q)
{
        if(q->size!=0)
        {
                q->size--;
                free(q->elements[q->front]);
                q->front++;
                //As we fill elements in circular fashion
                // if(q->front == q->capacity)
                //         q->front = 0;
        }
        return;
}

char* front(Queue *q)
{
        if(q->size!=0)
        {
                /* Return the element which is at the front*/
                return q->elements[q->front];
        }
        return NULL;
}

void enqueue(Queue *q , char *element)
{
        //If the Queue is full, we cannot push an element into it as there is no space for it.
        if(q->size == q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                q->size++;
                q->rear = q->rear + 1;
                //As we fill the queue in a circular way
                if(q->rear == q->capacity)
                {
                        q->rear = 0;
                }
                //Insert the element in its rear side

                printf("testing\n");

                q->elements[q->rear] = (char *) malloc((sizeof element + 1)* sizeof(char));

                strcpy(q->elements[q->rear], element);
        }
        return;
}