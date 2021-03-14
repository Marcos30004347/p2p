#include "async.h"

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

typedef void*(*thread_handler)(void*);

typedef struct thread {
    pthread_t object;
    thread_handler handler;
    void* arg;
} thread;

typedef struct mutex {
    pthread_mutex_t object;
} mutex;

thread* thread_create(thread_handler handler, void* arg) {
    thread *t = (thread*)malloc(sizeof(thread));


    t->handler = handler;
    t->arg = arg;

    pthread_create(&t->object, NULL, t->handler, t->arg);
    return t;
}

void thread_destroy(thread* t)
{
    free(t);
}

void thread_join(struct thread* t) {
    pthread_join(t->object, NULL);
}

mutex* mutex_create() {
    mutex* m = (mutex*)malloc(sizeof(mutex));
    pthread_mutex_init(&(m->object), NULL);
    return m;
}

void mutex_lock(mutex* m) {
    pthread_mutex_lock(&m->object);
}

void mutex_unlock(mutex* m) {
    pthread_mutex_unlock(&m->object);
}

void mutex_destroy(mutex* m) {
    free(m);
}