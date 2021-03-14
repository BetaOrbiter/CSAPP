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
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                 还妹有做出来                              */
/*                                  先放一放                                 */
/*                                  下次一定                                 */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
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

#define WSIZE 4U
#define DSIZE 8U
#define CHUNKSIZE (1U<<12)
#define MINBLOCKSIZE ALIGN((DSIZE + 2*sizeof(void*))) //最小块大小（空闲块头尾，指针）
#define FRFLAG 0x0U
#define ALFLAG 0x1U
#define PREALL 0x2U

#define MAX(x, y) ( ((x) > (y)) ? (x) : (y) )
#define MIN(x, y) ( ((x) < (y)) ? (x) : (y) )

#define GET(p) (*(unsigned *)(p))               //get the word *p
#define PUT(p, val) (*(unsigned *)(p) = (val))  //make word *p = val
#define PACK(size, flag) ((size) | (flag))    //pack the block size and allocated bit

#define GET_SIZE(p) (GET(p) & ~0x7)             //get size of block p
#define GET_ALLOC(p) (GET(p) & ALFLAG)          //if block p allocated
#define GET_PREV_ALLOC(p) (GET(p) & PREALL)

#define HDRP(pp) ((char *)(pp) - WSIZE)//get header from address of play_load
#define FTRP(pp) (HDRP(pp) - WSIZE + GET_SIZE(HDRP(pp)))//get foot from address of play_load
#define NEXT_BLKP(p) (((char *)(p)) + GET_SIZE(p))//get next 
#define PREV_BLKP(p) (((char *)(p)) - GET_SIZE(((char *)(p) - WSIZE)))

#define NEXT_PTR(p) (*(void **) ((char *)(p)+WSIZE + sizeof(void*)))
#define PREV_PTR(p) (*(void **) ((char *)(p)+WSIZE))
#define LISTNUM 16
static char *heap_listp = 0;
static void *segregated_free_list[LISTNUM];

//内部一律处理头部指针，整个块大小，只在输入输出时转换

static void *extend_heap(size_t size);//
static void *coalesce(void *ptr);
static void place(void *ptr, int size);

static inline void *find_fit(size_t size);//malloc 中查找合适的块
static void insert_block(void *ptr);//向链表组插入块
static void delete_block(void *ptr);//从链表组删除块
static inline size_t lowest_list(size_t size);//返回合适的链表
static void checker(void);
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    for (int i = 0; i < LISTNUM; i++)
        segregated_free_list[i] = NULL;
    heap_listp = mem_sbrk(4*WSIZE);
    if((void*)-1 == heap_listp)
        return -1;
    
    PUT(heap_listp, 0);
    PUT(heap_listp + WSIZE, PACK(8,ALFLAG));
    PUT(heap_listp + 2*WSIZE, PACK(8,ALFLAG));
    PUT(heap_listp + 3*WSIZE, PACK(0,ALFLAG|PREALL));
    heap_listp += DSIZE;

    if (NULL == extend_heap(CHUNKSIZE))
        return -1;
    else
        return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    if (0 == size)
        return NULL;

    size_t real_size = ALIGN(size + DSIZE);//加头部之后八字节对其
    real_size = MAX(real_size, MINBLOCKSIZE);//no smaller than minimum free block
    void* ptr;

    if (NULL == (ptr = find_fit(real_size)))
        if(NULL == (ptr = extend_heap(MAX(real_size, CHUNKSIZE))))
            return NULL;
    place(ptr, real_size);
    checker();
    return ptr + WSIZE;//return address of play_load instead of header
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    void* const header = HDRP(bp);//change to header pointer
    const unsigned val = GET(header) & ~ALFLAG;
    PUT(HDRP(bp), val);
    PUT(FTRP(bp), val);

    // insert_block(header);
    coalesce(header);
    checker();
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if(NULL == ptr)
        return NULL;
    if(0 == size){
        mm_free(ptr);
        return NULL;
    }

    void * const header = HDRP(ptr);//change to header pointer    
    size_t real_size = ALIGN(size+DSIZE);
    real_size = MAX(real_size, MINBLOCKSIZE);
    const size_t original_size = GET_SIZE(header);

    if (original_size >= real_size) {
        place(HDRP(ptr), real_size);
        return ptr;
    }
    else if(GET_ALLOC(NEXT_BLKP(header)) || (GET_SIZE(NEXT_BLKP(header)) < real_size-original_size)){
        void* new_playload_p = mm_malloc(size);
        memcpy(new_playload_p, ptr, original_size-WSIZE);
        mm_free(ptr);
        return new_playload_p;
    }
    else{
        delete_block(NEXT_BLKP(header));
        GET(header)+=GET_SIZE(NEXT_BLKP(header));
        GET(NEXT_BLKP(NEXT_BLKP(header))) = GET(NEXT_BLKP(NEXT_BLKP(header))) | PREALL;
        return ptr;
    }
    checker();
}

void* extend_heap(size_t size)
{
    size = ALIGN(size);
    void* ptr = mem_sbrk(size);
    if ((void *)-1 == ptr)
        return NULL;

    //注意原先的结束块，因此ptr指向新的负载
    PUT(HDRP(ptr), PACK(size, FRFLAG|GET_PREV_ALLOC(HDRP(ptr))));//注意原结束块保存的，原最后一个块的free/allocated信息
    PUT(FTRP(ptr), GET(HDRP(ptr)));
    PUT(NEXT_BLKP(HDRP(ptr)), PACK(0, ALFLAG));
    
    // insert_block(HDRP(ptr));
    return coalesce(HDRP(ptr));
}

void* coalesce(void* ptr)
{

    void * const pre_head = PREV_BLKP(ptr);
    void * const next_head = NEXT_BLKP(ptr);
    const size_t prev_alloc = GET_PREV_ALLOC(ptr);
    const size_t next_alloc = GET_ALLOC(next_head);

    if(prev_alloc && next_alloc)
        ;
    else if (!prev_alloc && next_alloc) {
        delete_block(pre_head);

        const size_t val = GET_SIZE(ptr) + GET(pre_head);//注意保留前一个块的PREALL
        PUT(pre_head,  val);
        PUT(FTRP(ptr + WSIZE), val);
        ptr = pre_head;
    }
    else if (prev_alloc && !next_alloc) {
        delete_block(next_head);

        const size_t val = GET(ptr) + GET_SIZE(next_head);//同上
        PUT(ptr, val);
        PUT(FTRP(ptr + WSIZE), val);
    }
    else{
        delete_block(pre_head);
        delete_block(next_head);

        const size_t val = GET(pre_head) + GET_SIZE(ptr) + GET_SIZE(next_head);
        PUT(pre_head, val);
        PUT(FTRP(next_head + WSIZE), val);
        ptr = pre_head;
    }
    insert_block(ptr);
    return ptr;
}

void place(void* ptr, int size)
{
    delete_block(ptr);
    const size_t prev_size = GET_SIZE(ptr);
    const size_t prev_alloc = GET_PREV_ALLOC(ptr);
    if (prev_size >= MINBLOCKSIZE + size) {
        PUT(ptr, PACK(size, ALFLAG | prev_alloc));
        ptr = NEXT_BLKP(ptr);
        PUT(ptr, PACK(prev_size - size, FRFLAG | PREALL));
        PUT(FTRP(ptr+WSIZE), PACK(prev_size - size, FRFLAG | PREALL));
        insert_block(ptr);
    }
    else{
        PUT(ptr, PACK(prev_size, ALFLAG | prev_alloc));
        ptr = NEXT_BLKP(ptr);
        PUT(ptr, GET(ptr) | PREALL);
    }
}

inline void* find_fit(size_t size)
{
    void* ptr = NULL;
    for (size_t index = lowest_list(size); index < LISTNUM; index++) {
        void* list = segregated_free_list[index];
        for (ptr = list; (NULL != ptr) && (GET_SIZE(ptr) < size);)
            ptr = NEXT_PTR(ptr);
        if (NULL != ptr)
            break;
    }
    return ptr;
}

void insert_block(void* ptr)
{
    const size_t size = GET_SIZE(ptr);
    void** list = &segregated_free_list[lowest_list(size)];
    void *pre = *list;
    void* nxt = pre;
    
    while ((NULL != nxt) && (GET_SIZE(nxt) < size)) {
        pre = nxt;
        nxt = NEXT_PTR(nxt);
    }

    if(NULL == pre){
        //没有节点
        *list = ptr;
        NEXT_PTR(ptr) = NULL;
        PREV_PTR(ptr) = NULL;
    }else{ 
        if(nxt == *list){
            //第一个插入
            *list = ptr;
            PREV_PTR(ptr) = NULL;
        }
        else{
            NEXT_PTR(pre) = ptr;
            PREV_PTR(ptr) = pre;
        }
        NEXT_PTR(ptr) = nxt;
        if(NULL != nxt)
            PREV_PTR(nxt) = ptr;
    }
}

void delete_block(void* ptr)
{
    void* pre = PREV_PTR(ptr);
    void* nxt = NEXT_PTR(ptr);
    if (NULL == pre)
        //注意ptr是第一个节点的情况
        for(size_t index = lowest_list(GET_SIZE(ptr)); index<LISTNUM; index++)
            if(segregated_free_list[index] == ptr)
                segregated_free_list[index] = nxt;
            else
                continue;
    else
        NEXT_PTR(pre) = nxt;

    if (NULL != nxt)
        PREV_PTR(nxt) = pre;
}

inline size_t lowest_list(size_t size)
{
    size_t index = 0;
    size >>= 3;
    while ((index < LISTNUM - 1) && (size > 1))
    {
        size >>= 1;
        index++;
    }
    return index;
}

void checker(void){
    if(*(unsigned*)0xf69d5f4c == 0x115b)
        exit(0);
    // if(*(char *)0xf69d70a0  == 0x5a)
    //     ;
    static size_t count = 0;
    count++;
    // int flg = 1;
    // for(size_t index=0; index<LISTNUM; index++)
    //     if(segregated_free_list[index] != NULL){
    //         void *pre, *nxt;
    //         pre = segregated_free_list[index];
    //         if(PREV_PTR(pre)!=NULL)
    //             flg = 0;

    //         for(nxt = NEXT_PTR(pre); NULL!=nxt;pre =nxt, nxt=NEXT_PTR(nxt))
    //             if((GET_SIZE(nxt)<GET_SIZE(pre)) || (PREV_PTR(nxt)!=pre) || (index!=lowest_list(GET_SIZE(pre))))
    //                 flg = 0;
    //         if(lowest_list(GET_SIZE(pre)) != index)
    //             flg = 0;
    //     }

    // for(void *pre=NEXT_BLKP(HDRP(heap_listp)); GET(pre) != 0x1; pre=NEXT_BLKP(pre))
    //     ;
    // if(flg == 0)
    // {
    //     printf("error at %u\n", count);
    //     exit(0);
    // }
}