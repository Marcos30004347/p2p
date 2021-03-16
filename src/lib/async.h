#ifndef NETWORK_ASYNC_H
#define NETWORK_ASYNC_H

typedef struct thread {
    pthread_t object;
    thread_handler handler;
    void* arg;
} thread;

typedef struct mutex {
    pthread_mutex_t object;
} mutex;

typedef void*(*thread_handler)(void*);

struct thread thread_create(thread_handler handler, void* arg);
void thread_join(struct thread* thread);

struct mutex mutex_create();
void mutex_lock(mutex* mutex);
void mutex_unlock(mutex* mutex);
#endif