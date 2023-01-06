#ifndef VoidQueueElectricSegFaultsToo
#define VoidQueueElectricSegFaultsToo
/*      Author: Magnus      */

/**
 * A queue which stores outgoing pointers that it does not own.
 * Programmer must ensure that the queue is deconstructed or the items at the least popped, before items are destroyed.
 * This struct should never be accessed directly, but through the given methods.
 */
struct linkedQueue {
    struct linkedQueueNode *toDequeue;
    struct linkedQueueNode **whereEnqueue;
    unsigned long numElements; //not strictly neccessary (check if toDequeue is 0 then it is empty) but it is nice
};
//operations: enqueue, dequeue, initialize, isEmpty, numItems, destroy

/**
 * Sets up the given linkedQueue to be used by the other methods in this file.
 */
void initLinkedQueue(struct linkedQueue *linkedQueue);

/**
 * Destroys all memory associated by this queue. Note this does not free the void pointers stored by this queue, as it owns them not.
 * Note that the memory associated with a single node is freed when it is dequeued.
 */
void destroyLinkedQueue(struct linkedQueue *linkedQueue);

/**
 * Returns 1 if the given queue is empty, 0 otherwise.
 */
char isEmptyQueue(struct linkedQueue *linkedQueue);

/**
 * Returns the number of items stored in this queue. Example: 0 right after being initialized.
 */
unsigned long itemsInQueue(struct linkedQueue *linkedQueue);

/**
 * Adds the given pointer to this queue.
 */
void enQueue(struct linkedQueue *lq, void* p);

/**
 * Removes the oldest pointer from this queue and returns it.
 * Frees the memory this data structure was using to store it.
 * Must check to ensure the queue has things on it before calling
 */
void* deQueue(struct linkedQueue *lq);

//private
/**
 * This node should never be accessed directly.
 * A node for a queue which stores outgoing pointers that it does not own.
 */
struct linkedQueueNode {
    void *data;
    struct linkedQueueNode *after;
};

/*
 * This should never be accessed directly
 * myMalloc that prints fail and exits when it fails to malloc
 */
void* mMllc (unsigned long bytes);

#endif
