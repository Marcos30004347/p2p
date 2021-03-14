#include "chunks.h"

chunks* chunks_create(short id, const char* path) {
    chunks* c = (chunks*)malloc(sizeof(chunks));
    c->id = id;
    c->path = (char*)malloc(strlen(path));
    strcpy(c->path, path);
    c->next = NULL;
    c->prev = NULL;
    return c;
}

void chunks_remove(chunks* c) {
    if(c->prev) c->prev->next = c->next;
    if(c->next) c->next->prev = c->prev;

    free(c->path);
    free(c);
}

chunks* chunks_add(chunks *h, chunks* c) {
    if(h == NULL) return c;

    if(h->next){
        h->next->prev = c;
        c->next = h->next;
    }

    h->next = c;
    c->prev = h;

    return h;
}

void chunks_destroy(chunks *h) {
    if(h == NULL) return;

    while(h->prev) {
        h = h->prev;
    }

    chunks* tmp = h->next;
    chunks_remove(h);
    chunks_destroy(tmp);
}
