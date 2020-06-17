#include "queue.h"

// Creates a poinetr to a new queue. Returns NULL if there was an error.

Queue* newQueue(){

    Queue* queue = (Queue*) malloc(sizeof(Queue));
    
    int res = sem_init(&(queue->semaphore), 0, 0);
    if(res!=0){
        //fprintf(2, "There was an error initializing the semaphore!\n");
        return NULL;
    }

    res = pthread_mutex_init(&(queue->mutex),NULL);

    if(res!=0){
        //fprintf(2, "There was an error initializing the mutex!\n");
        return NULL;
    }

    queue->head = NULL;
    queue->tail = NULL;

};


int dequeue(Queue* queue, void** object_repo){
    // This sem_wait() call blocks the function until there is at least one object in the queue
    sem_wait(&(queue->semaphore));

    pthread_mutex_lock(&(queue->mutex));
    void* tmp_data = queue->head->data;
    Node* tmp_node = queue->head;
    // Set the head of thenode to be the next node int he list
    queue->head = queue->head->next;
    // If there was only one object in the queue before dequeueing, the head and the tail of the 
    // would point to the same node. In this case we need to set the tail to NULL
    if(tmp_node==queue->tail){
        queue->tail =NULL;
    }
    // Destroy the Node we removed
    free(tmp_node);
    // Now we need to put the data one the object repo
    *object_repo = tmp_data;
    pthread_mutex_unlock(&(queue->mutex));
};

int enqueue(Queue* queue, void* obj){
    // If object is  NULL, return an error.
    if(obj == NULL){
        return -1;
    }
    //Create a Node to store the object in.
    Node* node = (Node*) malloc(sizeof(Node));
    node->data = obj;
    // Objects resently inserted don't have a "next" object
    node->next = NULL;
    
    pthread_mutex_lock(&(queue->mutex));
    // Objects are inserted at the tail.
    if(queue->head==NULL){
        queue->head = node;
        //queue->tail = node;
    }else{
        queue->tail->next = node;
        //queue->tail= node;
    }
    queue->tail = node;
    // Once we add an object to the queue, we can  do a sem_post() (increment the counter)
    // to allow another thread to dequeue from the queue.
    sem_post(&(queue->semaphore));
    pthread_mutex_unlock(&(queue->mutex));
    return 0;
};

void destroyQueue(Queue* queue){

    freeList(queue->head, queue->tail);

    sem_destroy(&(queue->semaphore));
    pthread_mutex_destroy(&(queue->mutex));

};

// Function used to free the linked list inside the Queue. IT DOESN'T FREE THE DATA INSIDE.
void freeList(Node * head, Node* tail){
    if(head==NULL){
        return;
    }

    Node* tmp_node=head;
    Node* next_node = tmp_node->next;
    
    while(tmp_node!=tail){
        free(tmp_node);
        tmp_node = next_node;
        next_node = next_node->next;
    }
    // Once we leave the WHILE() loop, the last node (the tail) still needs to be freed
    free(tmp_node);
}

// Returns true when the Queue is empty, false otherwise. Uses the internal lock!
bool isEmpty(Queue* queue){
    pthread_mutex_lock(&(queue->mutex));
    bool res = false;
    if(queue->head==NULL){
        res = true;
    }
    
    pthread_mutex_unlock(&(queue->mutex));
    return res;
};


/*
int main(){
    printf("Hello!\n");

    Queue* queue = newQueue();
    int* tmp = NULL;
    dequeue(queue,tmp);
    printf("Hi again\n");
    int *a = (int*)malloc(sizeof(int));
    *a =5;
    int *b = (int*)malloc(sizeof(int));
    *b = 2;
    int *c = (int*)malloc(sizeof(int));
    *c = 56;

    enqueue(queue, a);
    enqueue(queue, b);
    enqueue(queue, c);

    

    dequeue(queue, &tmp);
    printf("Value of number = %d\n", *tmp);

    dequeue(queue, &tmp);
    printf("Value of number = %d\n", *tmp);

    dequeue(queue, &tmp);
    printf("Value of number = %d\n", *tmp);

    dequeue(queue, &tmp);
    printf("Value of number = %d\n", *tmp);

    return 0;
    
}
*/