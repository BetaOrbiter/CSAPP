#include "../csapp.h"
static sem_t w;//资源锁
static int writer_cnt, reader_cnt;
static sem_t mutex;
static int read_time, write_time;
void *writer(void *vargp){
    while(1){
        if(writer_cnt >= 1)
            continue;
        P(&w);
        //read
        read_time++;
        V(&w);
    }
}

void *reader(void *vargp){
    while(1){
        P(&mutex);
        writer_cnt++;
        V(&mutex);

        P(&w);
        //write
        write_time++;
        V(&w);

        P(&mutex);
        writer_cnt--;
        V(&mutex);

    }
}

int main(void){
    Sem_init(&w, 0, 1);
}