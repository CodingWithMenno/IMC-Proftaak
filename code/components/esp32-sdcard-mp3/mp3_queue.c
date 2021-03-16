#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mp3_queue.h"

char* front(Queue *q)
{
        if (q == NULL)
                return NULL;
        

        Queue *first = q;
        if (first == NULL)
                return NULL;

        q = q->next;

        return first->data;
}

void enqueue(Queue *q , char *data)
{
        if (q->data == NULL)
        {
                q->data = data;
                return;
        }
        

        Queue *temp = q;
        while (temp->next != NULL)
        {
                temp = temp->next;
        }
        
        Queue *newNode = (Queue*) malloc(sizeof(Queue));
        temp->next = newNode;
        newNode->data = data;
        newNode->next = NULL;
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