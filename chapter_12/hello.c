#include "../csapp.h"

void *thread(void *vargp){
    printf("Hello, world\n");
    return NULL;
}

int main(int argc, char *argv[]){
    if(2 != argc){
        printf("Require arguments!\n");
        exit(0);
    }

    int n = atoi(argv[1]);
    pthread_t tid;
    for(int i=0;i<n;i++){
        Pthread_create(&tid, NULL, thread, NULL);
        Pthread_join(tid, NULL);
    }
    exit(0);
}