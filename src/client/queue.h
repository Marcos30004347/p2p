
#include "lib/async.h"
#include "protocol.h"

#include <stdlib.h>
#include <stdio.h>

short HANDLE_REQUEST = 1;
short HANDLE_WORK = 2;

typedef void(*work_handler)(udp_client* client);

typedef struct work {
    work_handler handler;
} work;

typedef struct node {
    struct node* prev;
    struct node* next;

    short type;
    request* request;
    work* work;
} node;

typedef struct queue {
    mutex lock;
    node* head;
    node* tail;
} queue;

queue* queue_init();
void queue_destroy(queue* q);
void queue_push(queue* q, short type, void* payload);
node* queue_pop(queue* queue);