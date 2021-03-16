#ifndef CHUNKS_H
#define CHUNKS_H

#include<stdlib.h>
#include<string.h>

typedef struct chunks {
    short id;
    char* path;
    struct chunks* next;
    struct chunks* prev;
} chunks;

chunks* chunks_create(short id, const char* path);
void chunks_remove(chunks* c);

chunks* chunks_add(chunks *h, chunks* c);
void chunks_destroy(chunks *h);

#endif