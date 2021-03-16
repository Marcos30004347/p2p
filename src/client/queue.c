
#include "lib/async.h"
#include "protocol.h"
#include "queue.h"

#include <stdlib.h>
#include <stdio.h>

queue* queue_init() {
    queue* q = (queue*)malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    q->lock = mutex_create();
}

void queue_destroy(queue* q) {
    mutex_destroy(q->lock);
    free(q);
}

void queue_push(queue* q, short type, void* payload) {
    mutex_lock(&(q->lock));

    node* n = (node*)malloc(sizeof(node));
    n->type = type;

    n->prev = q->tail;
    n->next = NULL;

    if(type == HANDLE_REQUEST)
        n->request = (request*)payload;    
    else if(type == HANDLE_WORK)
        n->work = (work*)payload;
    else {
        printf("Unknown node node type\n");
        exit(EXIT_FAILURE);
    }

    if(q->tail)
        q->tail->next = n;

    if(!q->head)
        q->head = n;

    q->tail = n;

    mutex_unlock(&(q->lock));
}

node* queue_pop(queue* queue) {
    mutex_lock(&(queue->lock));

    node* tmp = queue->head;
    queue->head = queue->tail;

    mutex_unlock(&(queue->lock));
    return tmp;
}

void node_free(node* q) {
    if(q->request)
        destroy_request((request*)q->request);
    if(q->work)
        free((work*)q->work);
    free(q);
}