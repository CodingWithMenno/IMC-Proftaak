#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp3_queue.h"

char* front(Queue **q)
{
        if (*q == NULL)
                return NULL;
        
        Queue *first = *q;
        char *data = first->data;

        (*q) = (*q)->next;
        free(first);

        return data;
}

void enqueue(Queue **q , char *data)
{
        Queue *newNode = (Queue*) malloc(sizeof(Queue));
        Queue *last = *q;

        newNode->data = data;
        newNode->next = NULL;

        if (*q == NULL)
        {
                *q = newNode;
                return;
        }

        while (last->next != NULL)
                last = last->next;
        
        last->next = newNode;
        return;
}

void freeQueue(Queue *q)
{
        Queue *current = q;
        Queue *next;
        while (current != NULL)
        {
                next = current->next;
                free(current);
                current = next;
        }

        q = NULL;
}