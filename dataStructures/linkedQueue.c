/*      Author: Magnus      */

#include <stdio.h>
#include <stdlib.h>
#include "linkedQueue.h"

void initLinkedQueue(struct linkedQueue *linkedQueue) {
    linkedQueue->toDequeue=0;
    linkedQueue->whereEnqueue=&(linkedQueue->toDequeue);
    linkedQueue->numElements=0;
}

void destroyLinkedQueue(struct linkedQueue *linkedQueue) {
    struct linkedQueueNode *toFree = linkedQueue->toDequeue;
    while (toFree) {
        toFree = toFree->after;
        free(linkedQueue->toDequeue);
        linkedQueue->toDequeue = toFree;
    }
}

char inline isEmptyQueue(struct linkedQueue *linkedQueue) {
    return !linkedQueue->toDequeue;
}

unsigned long inline itemsInQueue(struct linkedQueue *linkedQueue) {
    return linkedQueue->numElements;
}

void enQueue(struct linkedQueue *lq, void* p) {
    *(lq->whereEnqueue) = mMllc(sizeof(struct linkedQueueNode));
    (*(lq->whereEnqueue))->after = 0;
    (*(lq->whereEnqueue))->data = p;
    lq->whereEnqueue=&((*(lq->whereEnqueue))->after);
    lq->numElements++;
}

void* deQueue(struct linkedQueue *lq) {
    if (isEmptyQueue(lq)) {
        fprintf(stderr,"Programmer error by something using a queue (Try to dequeue an empty queue)");
        exit(1);
    }
    struct linkedQueueNode *dequeueing = lq->toDequeue;
    lq->toDequeue=lq->toDequeue->after;
    if (!lq->toDequeue) lq->whereEnqueue=&(lq->toDequeue);
    void *retval = dequeueing->data;
    free(dequeueing);
    lq->numElements--;
    return retval;
}

void* mMllc (unsigned long bytes) {
    void* retval = malloc(bytes);
    if(!retval) {
        fprintf(stderr,"myMalloc fail for %lu bytes!\n", bytes);
        exit(1);
    }
    return retval;
}

/**
 * Main program to test this queue;
* /
int main(int lmao) {
    struct linkedQueue myQ;
    initLinkedQueue(&myQ);
    unsigned i;
    printf("Queue is empty: %s\n",(isEmptyQueue(&myQ))?"true":"false");
    for(i=0; i<1;i++) enQueue(&myQ,(void*)0x42);
    printf("Queue is empty: %s\n",(isEmptyQueue(&myQ))?"true":"false");
    for(i=0; i<10; i++) enQueue(&myQ,(void*)i);
    for(i=0; i<4; i++) printf("%p ",deQueue(&myQ)); printf("\n");
    for(i=0; i<10 + 4000; i++) {enQueue(&myQ,(void*)(i+10)); if (isEmptyQueue(&myQ)) printf("\n\nYOU SUCK DUDE\n\n");}
    for(i=0; i<17 + 2000; i++) printf("%p ",deQueue(&myQ)); printf("\n");
    printf("Queue is empty: %s\n",(isEmptyQueue(&myQ))?"true":"false");
    destroyLinkedQueue(&myQ);
    printf("Queue is empty: %s\n",(isEmptyQueue(&myQ))?"true":"false");
}*/
