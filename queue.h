#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

// Structure that represent the nodes in the linked list.
typedef struct node{
    void* data;
    struct node *next;

} Node;
// Structure that represents the Blocking Queue, which is implemented as a Linked List
typedef struct queue{
    // Head of the queue. Here lies the object to dequeue next.
    Node *head;
    // Tail of the queue. Here lies the object that was last inserted into the queue.
    Node *tail;
    //NUmber of elements in the Queue. Might not be needed.
    int length;
    // semaphore used to control the access to the queue. This semaphore is what makes the queue block when using the dequeue() function.
    sem_t semaphore;
    // Mutex lock thats used when queueing/dequeueing. Difference with the semaphore:
    // On dequeueing, the semaphore blocks the function call if there are no items to dequeue. Once there is at least one item
    // available, the mutex is used to avoid race conditions when actually dequeueing the object.
    pthread_mutex_t mutex;
} Queue;

//API

Queue* newQueue();

int dequeue(Queue* queue, void** object_repo);

int enqueue(Queue* queue, void* obj);

void destroyQueue(Queue* queue);

bool isEmpty(Queue* queue);