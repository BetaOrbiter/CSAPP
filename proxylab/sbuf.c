#include "sbuf.h"
#include "csapp.h"
#include <semaphore.h>
#include <stdatomic.h>

sbuf_t* sbuf_init(int n){
    sbuf_t* sp = Malloc(sizeof(sbuf_t) + n * sizeof(int));
    sp->n = n;
    sp->front = sp->rear = 0;
    Sem_init((sem_t*)&sp->slots, 0, n);
    Sem_init((sem_t*)&sp->items, 0, 0);

    return sp;
}

void sbuf_destory(sbuf_t* sp){
    Sem_destroy(&sp->slots);
    Sem_destroy(&sp->items);
    Free(sp);
}

void sbuf_push(sbuf_t *sp, int item){
    P((sem_t*)&sp->slots);
    //单生产者不需要原子操作
    sp->buf[(sp->rear++) % (sp->n)] = item;
    V((sem_t*)&sp->items);
}

int sbuf_pop(sbuf_t *sp){
    P((sem_t*)&sp->items);
    uint pos = atomic_fetch_add(&sp->front, 1) % sp->n;
    int item = sp->buf[pos];
    V((sem_t*)&sp->slots);
    return item;
}