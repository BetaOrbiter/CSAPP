#include <semaphore.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <sys/types.h>
/**
 * 单生产者多消费者的环形缓冲区
*/
typedef struct sbuf_t{
    //空闲槽位
    sem_t slots;
    //当前元素数量
    sem_t items;
    //元素数量
    int n;
    //头索引
    atomic_uint front;
    //尾索引
    uint rear;
    //缓冲数组
    int buf[];
}sbuf_t;

sbuf_t* sbuf_init(int n);
void sbuf_destory(sbuf_t* sp);
void sbuf_push(sbuf_t* sp, int item);
int sbuf_pop(sbuf_t* sp);