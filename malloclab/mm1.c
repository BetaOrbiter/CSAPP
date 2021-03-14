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

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define FRFLAG 0x0
#define ALFLAG 0x1
#define PREALL 0x2

#define MAX(x, y) ( ((x) > (y)) ? (x) : (y) )
#define MIN(x, y) ( ((x) < (y)) ? (x) : (y) )

#define GET(p) (*(unsigned *)(p))               //get the word *p
#define PUT(p, val) (*(unsigned *)(p) = (val))  //make word *p = val
#define GET_SIZE(p) (GET(p) & ~0x7)             //get size of block p
#define GET_ALLOC(p) (GET(p) & ALFLAG)          //if block p allocated
#define GET_PREV_ALLOC(p) (GET(p) & PREALL)
#define PACK(size, alloc) ((size) | (alloc))    //pack the block size and allocated bit

#define HDRP(bp) ((char *)(bp) - WSIZE)         //get header pointer of play load
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)//get foot pointer of play load

#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //line:vm:mm:nextblkp
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //line:vm:mm:prevblkp

static inline size_t min_alloc_size(size_t size);
static inline size_t min_free_size(void);
static void *coalesce(void *bp);
static void *extend_heap(size_t words_num);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

static char *heap_listp = 0;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if((void*)-1 ==(heap_listp = mem_sbrk(4 * WSIZE)))
        return -1;

    PUT(heap_listp, 0);
    PUT(heap_listp + WSIZE, PACK(DSIZE, ALFLAG));
    PUT(heap_listp + DSIZE, PACK(DSIZE, ALFLAG));
    PUT(heap_listp + 3*WSIZE, PACK(0, ALFLAG|PREALL));
    heap_listp += (2*WSIZE);
    if(NULL == extend_heap(CHUNKSIZE/WSIZE))
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t real_size;
    char *bp;

    if(0 == size)
        return NULL;
    
    real_size = min_alloc_size(size);

    if(NULL == (bp = find_fit(real_size))){
        size_t extend_size = MAX(real_size, CHUNKSIZE);
        if(NULL == (bp = extend_heap(extend_size)))
            return NULL;
    }
    place(bp, real_size);
    return bp;

}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    const void * const header = HDRP(bp);
    const void * const foot = FTRP(bp);

    const size_t val = PACK(GET_SIZE(header), FRFLAG|GET_PREV_ALLOC(header));
    PUT(header, val);
    PUT(foot, val);
    
    void *next_header = HDRP(NEXT_BLKP(bp));
    PUT(next_header, GET(next_header) & ~PREALL);//tell the next block free
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if(0 == size){
        mm_free(ptr);
        return NULL;
    }
    if(NULL == ptr)
        return mm_malloc(size);

    size_t old_size = GET_SIZE(HDRP(ptr));
    size_t new_size = min_alloc_size(size);
    if(new_size <= old_size){
        place(ptr, new_size);
        return ptr;
    }
    else{
        void *new_bp = mm_malloc(size);
        if(NULL == new_bp)
            return NULL;
        
        memcpy(new_bp, ptr, old_size-WSIZE);
        mm_free(ptr);
        return new_bp;
    }

}


void *coalesce(void *bp){
    const size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    // size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    const size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc)
        return bp;
    else if(prev_alloc && !next_alloc){
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, FRFLAG | PREALL));
        PUT(FTRP(bp), PACK(size, FRFLAG | PREALL));
        return bp;
    }
    else if(!prev_alloc && next_alloc){
        const void *pre_bp = PREV_BLKP(bp);
        const void *pre_head = HDRP(PREV_BLKP(bp));
        const size_t val = GET(pre_head) + size;
        PUT(pre_head, val);
        PUT(FTRP(bp), val);
        return pre_bp;
    }
    else{
        const void *pre_bp = PREV_BLKP(bp);
        const void *next_bp = NEXT_BLKP(bp);
        const void *pre_head = HDRP(pre_bp);
        const size_t val = GET(pre_head) + size + GET_SIZE(HDRP(next_bp));
        PUT(pre_head, val);
        PUT(FTRP(next_bp), val);
        return pre_bp;
    }
}

static void *extend_heap(size_t words_num){
    size_t real_size;
    void *bp;

    real_size = words_num*WSIZE + (words_num&1)*WSIZE;
    if((long)(bp = mem_sbrk(real_size)) == -1)
        return NULL;
    
    size_t pre_pre_alloc = GET_PREV_ALLOC(HDRP(bp));
    PUT(HDRP(bp), PACK(real_size, FRFLAG|pre_pre_alloc));
    PUT(FTRP(bp), PACK(real_size, FRFLAG));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, ALFLAG));

    return coalesce(bp);
}

static void *find_fit(size_t asize){
    void *bp;
    for(bp = heap_listp; 0 != GET_SIZE(HDRP(bp)); bp = NEXT_BLKP(bp)){
        void * header = HDRP(bp);
        if(!GET_ALLOC(header) && (GET_SIZE(header) >= asize))
            return bp;
    }
    return NULL;
}

static void place(void *bp, size_t asize){
    const size_t csize = GET_SIZE(HDRP(bp));

    const size_t pre_alloc = GET_PREV_ALLOC(HDRP(bp));
    if(csize >= asize + min_free_size()){//rest of origional block is big enough
        PUT(HDRP(bp), PACK(asize, ALFLAG|pre_alloc));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, FRFLAG | PREALL));
        PUT(FTRP(bp), PACK(csize-asize, FRFLAG | PREALL));
    }
    else{
        PUT(HDRP(bp), PACK(csize, ALFLAG | pre_alloc));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), GET(HDRP(bp)) | PREALL);
    }

}

static inline size_t min_alloc_size(size_t size){
    return DSIZE*((size+WSIZE+(DSIZE-1))/DSIZE);
}
static inline size_t min_free_size(void){
    return 2*DSIZE;
}