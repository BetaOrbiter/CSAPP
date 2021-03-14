#include "../csapp.h"

sem_t w;//写锁

size_t reader_cnt = 0;
sem_t reader_block;
void reader(void){
    while(1){
        P(&reader_block);
        if(1 == ++reader_cnt)
            P(&w);
        V(&reader_block);

        //read

        P(&reader_block);
        if(0 == --reader_cnt)
            V(&w);
        V(&reader_block);
    }
}
void writer(void){
    while(1){
        if(reader_cnt >= 1)
            continue;
        P(&w);

        //write

        V(&w);
    }
}