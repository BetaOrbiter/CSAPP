#include "../csapp.h"

//假装这里有个资源
//假装操作了它
#define WRITE
#define READ
static sem_t mutex,w;//资源锁

#define N 5
pthread_t reader_list[N];
int read_cnt_list[N];
void *reader(void *vargp){
    while(1){
        P(&mutex);
        P(&w);
        READ;
        V(&w);
        V(&mutex);

        (*(int*)vargp)++;
    }
}
void *writer(void *vargp){
    int write_cnt=0;
    while(1){
        P(&w);
        WRITE;
        V(&w);

        write_cnt++;
        if(write_cnt >= 1000000){
            int read_cnt=0;
            for(int i=0;i<N;i++){
                // Pthread_join(reader_list[i], NULL);
                read_cnt+=read_cnt_list[i];
            }
            printf("read/write: %d/%d\n", read_cnt, write_cnt);
            _exit(0);
        }
    }
}

void init(void){
    Sem_init(&mutex, 0, N);
    Sem_init(&w, 0, 1);
}

int main(void){
    init();
    pthread_t tid;

    for(int i=0; i<N;i++)
        Pthread_create(&reader_list[i], NULL, reader, &read_cnt_list[i]);
    Pthread_create(&tid, NULL, writer, NULL);

    Pthread_exit(NULL);
}