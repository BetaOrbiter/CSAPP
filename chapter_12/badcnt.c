#include "../csapp.h"

void *thread(void *vargp);

volatile long cnt = 0;
sem_t block;

int main(int argc, char **argv){
    long niters;
    pthread_t tid1, tid2;

    if(2 != argc){
        fprintf(stderr, "usage: %s <niters>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    niters = atoi(argv[1]);

    Sem_init(&block, 0, 1);
    Pthread_create(&tid1, NULL, thread, &niters);
    Pthread_create(&tid2, NULL, thread, &niters);
    Pthread_join(tid1, NULL);
    Pthread_join(tid2, NULL);

    if(cnt != (2*niters))
        printf("Fuck cnt = %ld\n", cnt);
    else
        printf("OK cnt = %ld\n", cnt);
    return 0;
}

void *thread(void *vargp){
    long i, niters = *((long*)vargp);
    
    for(i = 0; i < niters; i++){
        P(&block);
        cnt++;
        V(&block);
    }

    return NULL;
}