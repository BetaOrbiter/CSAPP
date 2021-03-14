#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "cachelab.h"

const char *help_message = 
    "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
    "Options:\n"
    "  -h         Print this help message.\n"
    "  -v         Optional verbose flag.\n"
    "  -s <num>   Number of set index bits.\n"
    "  -E <num>   Number of lines per set.\n"
    "  -b <num>   Number of block offset bits.\n"
    "  -t <file>  Trace file.\n\n"
    "Examples:\n"
    "  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
    "  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";

struct Line{
    _Bool valid_bit;
    unsigned counter;
    uint64_t tag; 
};
typedef struct Line* Set;

uint64_t s=-1,E=-1,b=-1;
_Bool megFlg=false;
FILE *tracesFile=NULL;

void setArg(int argc, char **argv);
Set* allocate(void);
void freeMem(Set* set);
struct Line* choose(uint64_t tag, struct Line set[]);
void step(struct Line set[]);
int main(int argc, char **argv)
{
    setArg(argc, argv);
    Set *const sets = allocate();
    if(NULL == sets){
        puts("memory allocation failed.");
        exit(EXIT_FAILURE);
    }
    
    uint64_t missCount=0,hitCount=0,eviction=0;
    const uint64_t co = (1<<b)-1,ci = ((1<<s)-1)<<b,ct=0xffffffffffffffff^(co|ci);
    printf("%lx,%lx,%lx\n",co,ci,ct);
    char op;uint64_t add;
    while(2 == fscanf(tracesFile, " %c %lx%*[^\n]", &op,&add)){
        if('I' == op)
            continue;
        else
        {
            const uint64_t setIndex = (add&ci)>>b;
            const uint64_t tag = add&ct;
            struct Line* const victim = choose(tag,sets[setIndex]);

            _Bool hit=(true == victim->valid_bit) && (victim->tag == tag);
            hitCount += hit + ('M'==op);
            missCount += !hit;
            eviction += victim->valid_bit && !hit;

            if(megFlg){
                // printf("set index:%lu, tag:%lu\n",setIndex,tag);
                printf(hit?"hit ":"miss ");
                if(victim->valid_bit && !hit)
                    printf("eviction ");
                if('M' == op)
                    printf("hit");
                putchar('\n');
            }

            victim->valid_bit = true;
            victim->counter = 0;
            victim->tag = tag;
        
            step(sets[setIndex]);
        }
    }
    freeMem(sets);
    fclose(tracesFile);
    // printf("hit:%lu,miss:%lu,eviction:%lu\n",hitCount,missCount,eviction);
    printSummary(hitCount, missCount, eviction);
    return 0;
}

void setArg(int argc, char **argv){
    int op;const uint64_t S = 1<<s, B=1<<b;
    while (-1 != (op = getopt(argc,argv,"hvs:E:b:t:")))
    {
        switch (op)
        {
        case 'h':
            printf("%s",help_message);
            break;
        case 'v':
            megFlg = true;
            break;
        case 's':
            if((s = atol(optarg)) <= 0){
                puts("There are at least one set!");
                exit(EXIT_FAILURE);
            }
            break;
        case 'E':
            if((E = atol(optarg)) <= 0){
                puts("There are at least one line in each set");
                exit(EXIT_FAILURE);
            }
            break;
        case 'b':
            if((b = atol(optarg))<=0){
                puts("There are at least one Byte in each line");
                exit(EXIT_FAILURE);
            }
            break;
        case 't':
            if(NULL == (tracesFile = fopen(optarg,"r"))){
                printf("file name:%s\n",optarg);
                puts("file name missing or uncorrect!");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            printf("You fucking stupid\n");
            exit(EXIT_FAILURE);
        }
    }
    if(-1==S||-1==B||-1==E||NULL==tracesFile){
        puts("missing argument");
        exit(EXIT_FAILURE);
    }
}

Set* allocate(void){
    const int S = 1<<s;
    Set *ptr = (Set*)malloc(S * sizeof(struct Line*));
    int i;_Bool flg=true;
    for(i=0;i<S;i++)
        if(NULL == (ptr[i] = (struct Line*)calloc(sizeof(struct Line),E))){
            flg = false;
            break;
        }
    if(false == flg){
        for(int j=0;j<i;j++)
            free(ptr[j]);
        free(ptr);
        ptr = NULL;
    }
    return ptr;
}

void freeMem(Set* set){
    const uint64_t S=1<<s;
    for(uint64_t i = 0; i<S;i++)
        free(set[i]);
    free(set);
}

struct Line* choose(uint64_t tag, struct Line set[]){
    uint64_t lruIndex=-1,emptyIndex=-1;
    uint64_t maxCount=0;
    for(int i=0;i<E;i++){
        if(set[i].valid_bit==false){
            emptyIndex = i;
            continue;
        }
        if(set[i].tag == tag){
            return set + i;
        }
        if(set[i].counter > maxCount){
            lruIndex = i;
            maxCount = set[i].counter;
        }
    }
    if(-1 != emptyIndex)
        return set + emptyIndex;
    else
        return set + lruIndex;
}

inline void step(struct Line set[]){
    for(uint64_t i=0;i<E;i++)
        set[i].counter++;
}