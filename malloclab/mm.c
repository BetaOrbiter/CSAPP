/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */

/****************************************************************/
/*                         与课本不同                            */
/*                  显式空闲链表带头结点，放在堆头                 */
/*              使用了双链表结构体,链表操作方便                    */
/*              不使用指向负载的指针，只在接口处转换                */
/*         所有大小均为整块大小(而非负载)，亦在接口处转换            */
/*                  一定要时刻注意这两种区别                       */
/****************************************************************/
#include <stdio.h>
#include <stdint-gcc.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define LIST_MAX  (10)                              //空闲链表数量
#define WSIZE (4)                                   //字大小
#define DSIZE (8)                                   //双字大小
#define CHUNKSIZE (1<<12)                           //页大小
#define PTR_SIZE (sizeof(void*))                    //显式空闲链表指针大小
#define MIN_BLK_SIZE (ALIGN(2*WSIZE+2*PTR_SIZE))    //最小块大小(最小空闲块所需大小)
#define FRFLAG (0x0)                                //空闲块标志
#define ALFLAG (0x1)                                //已分配标志
#define PREALL (0x2)                                //前方已分配标志

#define GET(head)           (*(unsigned*)(head))
#define GET_SIZE(head)      (GET(head) & ~0x3)
#define GET_MASK(head)      (GET(head) & 0x3)
#define GET_ALLOC(head)     (GET(head) & ALFLAG)
#define GET_PRE_ALLOC(head) (GET(head) & PREALL)
#define PACK(size, mask)    ((size)|(mask))
#define PUT(ptr, val)       ((*(unsigned*)(ptr)) = (val))

#define HEAD(playload_ptr)      ((char*)(playload_ptr) - WSIZE)                   //负载-》头部
#define LOAD(head)              ((char*)(head) + WSIZE)                           //头部-》负载
#define FOOT(free_head)         (NEXT_HEAD(free_head) - WSIZE)                  //头部-》尾部
#define NEXT_HEAD(head)         ((char*)(head) + GET_SIZE(head))                  //头部-》下一个块头部
#define PREV_HEAD(head)         ((char*)(head) - GET_SIZE((char*)head-WSIZE))                  //头部-》上一个块头部（必须是空闲）

#define MIN(x,y)        (((x)<(y))?(x):(y))
#define MAX(x,y)        (((x)<(y))?(y):(x))

typedef struct node{
    //为链表操作方便，将头部强转为双链表结构体
    //题目不允许但我实在是受不了了
    unsigned block_size_mask;
    void *prev_node;
    void *next_node;
}node;
static char * heap_listp = NULL;
static node *node_listp  = NULL;

static void* extend_heap(size_t block_size);        //扩展堆大小，失败返回NULL,成功返回新空块头地址
static void* coalesce(void *head);                  //合并前后可能的空闲块，返回合并后头地址
static void  place(void *head, size_t block_size);  //分割空闲块，得到忙碌块和剩余部分(如有),不检查是否足够

//链表操作
static node* find_fit(size_t block_size);       //返回大于参数的最小空闲块，失败返回NULL
static void delete(struct node* ptr);           //从链表中删除节点
static void insert(struct node* ptr);           //把节点插入链表

int mm_init (void)
{
    if(-1 == (node_listp = mem_sbrk(4*WSIZE + LIST_MAX * sizeof(node))))
        return -1;
    
    for(size_t i=0;i<LIST_MAX-1;i++){
        node_listp[i].block_size_mask = (MIN_BLK_SIZE<<i);
        node_listp[i].prev_node = NULL;
        node_listp[i].next_node = NULL;
    }
    node_listp[LIST_MAX-1].block_size_mask = UINT32_MAX;//最后的链表无穷大
    node_listp[LIST_MAX-1].prev_node = NULL;
    node_listp[LIST_MAX-1].next_node = NULL;

    heap_listp = node_listp+LIST_MAX;
    heap_listp += WSIZE;
    PUT(heap_listp, PACK(8,ALFLAG|PREALL));
    PUT(heap_listp+WSIZE, PACK(8,ALFLAG|PREALL));
    PUT(heap_listp+DSIZE, PACK(0, ALFLAG|PREALL));
    if(NULL == extend_heap(CHUNKSIZE))
        return -1;
    return 0;
}
void* mm_malloc (size_t size)
{
    if(0 == size)
        return NULL;
    size_t block_size = ALIGN(size+WSIZE);
    block_size = MAX(block_size, MIN_BLK_SIZE);
    void * bp = find_fit(block_size);
    if(NULL == bp)
        if(NULL == (bp = extend_heap(MAX(block_size,CHUNKSIZE))))
            return NULL;
        else
            bp = find_fit(block_size);
    place(bp, block_size);

    return bp+WSIZE;
}
void  mm_free (void *ptr)
{
    if(NULL == ptr)
        return ;
    void * const head = HEAD(ptr);
    PUT(head, GET(head)&~ALFLAG);
    PUT(FOOT(head), GET(head));
    void * const next_head = NEXT_HEAD(head);
    PUT(next_head, GET(next_head)&~PREALL);
    if(!GET_ALLOC(next_head))
        PUT(FOOT(next_head), GET(next_head));
    coalesce(head);
}
void* mm_realloc(void *ptr, size_t size)
{
    if(0 == size){
        mm_free(ptr);
        return NULL;
    }
    if(size + WSIZE <= GET_SIZE(HEAD(ptr)))//新负载加头部小于原块大小
        return ptr;
    
    void * const head = HEAD(ptr);
    void * const next_head = NEXT_HEAD(head);
    if(GET_ALLOC(NEXT_HEAD(head)) || 
    (size+WSIZE > GET_SIZE(head)+GET_SIZE(next_head))){
        void *new_ptr = mm_malloc(size);
        memcpy(new_ptr, ptr, GET_SIZE(head)-WSIZE);
        mm_free(ptr);
        return new_ptr;
    }
    else{
        const size_t new_block_size = MAX(ALIGN(size+WSIZE), MIN_BLK_SIZE);
        const size_t require_size = new_block_size - GET_SIZE(head);
        place(next_head, require_size);
        PUT(head, PACK(new_block_size, GET_MASK(head)));
        return ptr;
    }
}

void* extend_heap(size_t block_size){
    block_size = ALIGN(block_size);
    block_size = MAX(block_size,MIN_BLK_SIZE);
    void *bp;
    if(-1 == (bp = mem_sbrk(block_size)))
        return NULL;
    
    bp = HEAD(bp);
    PUT(bp,             PACK(block_size,GET_PRE_ALLOC(bp)));
    PUT(FOOT(bp),       PACK(block_size,GET_PRE_ALLOC(bp)));
    PUT(NEXT_HEAD(bp),  PACK(0,ALFLAG));

    return coalesce(bp);
}

void* coalesce(void* head){
    unsigned block_size = GET_SIZE(head);
    unsigned mask = GET(head) & 0x3;

    if(!GET_ALLOC(NEXT_HEAD(head))){
        delete((node*)NEXT_HEAD(head));
        block_size += GET_SIZE(NEXT_HEAD(head));
    }
    if(!GET_PRE_ALLOC(head)){
        delete((node*)PREV_HEAD(head));
        block_size += GET_SIZE(PREV_HEAD(head));
        mask       |= GET_PRE_ALLOC(PREV_HEAD(head));
        head        = PREV_HEAD(head);
    }
    PUT(head,       PACK(block_size, mask));
    PUT(FOOT(head), PACK(block_size, mask));
    insert((node*)head);

    return head;
}

void  place(void *head, size_t block_size){
    //to speed up, i use few marcos
    delete((node*)head);
    const unsigned content = GET(head);
    const size_t left_size = (content&~0x3) - block_size;
    if(left_size < MIN_BLK_SIZE){
        PUT(head, PACK(content,ALFLAG));
        void * const next_head = (char*)head + (content&~0x3);
        const unsigned next_content = GET(next_head);
        PUT(next_head,PACK(next_content, PREALL));
        if(!(next_content&ALFLAG))
            PUT((char*)next_head + (next_content&~0x3) - WSIZE, content);
    }
    else{
        PUT(head, PACK(block_size,(PREALL&content)|ALFLAG));
        void * const next_head = (char*)head + block_size;
        PUT(next_head,                          PACK(left_size, PREALL));
        PUT((char*)next_head+left_size-WSIZE,   PACK(left_size, PREALL));
        insert((node*)next_head);
    }
}

inline void delete(node* ptr){
    node * const pre = ptr->prev_node;
    node * const nxt = ptr->next_node;
    pre->next_node = nxt;
    if(NULL != nxt)
        nxt->prev_node = pre;
}

void insert(struct node* ptr){
    //to speed up, i use few marcos
    node *pre, *nxt;
    const unsigned block_size = GET_SIZE(ptr);
    for(pre=node_listp;(pre->block_size_mask&~0x3) <block_size;pre++)
        continue;//确定放在哪个链表，注意头节点size字段无掩码而ptr有
    nxt = pre->next_node;
    //确定节点位置
    while(NULL!=nxt && (nxt->block_size_mask&~0x3)<block_size){
        pre = nxt;
        nxt = nxt->next_node;
    }
    pre->next_node = ptr;
    ptr->prev_node = pre;
    ptr->next_node = nxt;
    if(NULL !=  nxt)
        nxt->prev_node = ptr;
}

node* find_fit(size_t block_size){
    node * ret = NULL;
    size_t i;
    for(i=0;node_listp[i].block_size_mask < block_size && i<LIST_MAX; i++)
        continue;//筛掉不可能的链表组
    
    for(;i<LIST_MAX;i++){
        node * const head = node_listp + i;
        for(node*ptr = head->next_node; NULL!=ptr; ptr=ptr->next_node)
            if((ptr->block_size_mask&~0x3) >= block_size){
                ret = ptr;
                break;
            }
        if(NULL != ret)
            break;
    }
    return ret;
}
static void mem_dump(int mode){
    if(0 == mode){
        for(int i=0;i<LIST_MAX;i++){
            if(NULL == node_listp[i].next_node)
                continue;
            printf("size between %u and %u\n", node_listp[i].block_size_mask>>1, node_listp[i].block_size_mask);
            for(node *p=node_listp[i].next_node;p!=NULL;p=p->next_node)
                printf("%p %-10u %-10p %-10p\n", p, (p->block_size_mask&~0x3) , p->prev_node, p->next_node);
        }
    }
    else{
        for(void *p=heap_listp;0!=GET_SIZE(p);p=NEXT_HEAD(p)){
            printf("%p size:%-5u %s pre_%s ",p , GET_SIZE(p), 
                GET_ALLOC(p)?"busy":"free", GET_PRE_ALLOC(p)?"busy":"free");
            if(0 == GET_ALLOC(p))
                printf(GET(p)==GET(FOOT(p))?"foot right":"foot error");
            putchar('\n');
        }
    }
    puts("\n\n");
}