#include "../csapp.h"

void *thread(void *vargp){
    Sleep(1);
    printf("Hello, world!\n");
    return NULL;
}

int main(void){
    pthread_t tid;
    Pthread_create(&tid, NULL, thread, NULL);
    Pthread_join(tid,NULL);
}