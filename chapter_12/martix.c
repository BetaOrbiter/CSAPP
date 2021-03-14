#include "../csapp.h"

#define N 4096
#define M 2048
#define THREAD_NUM 4
#define N_PRE_THREAD ((N)/(THREAD_NUM))
typedef unsigned elementtype;

elementtype A[N][M];//former
elementtype B[M][N];//later
elementtype C[N][N];//result
elementtype D[N][N];

void init_martix(void){
    for(int i=0; i<N; i++)
        for(int j=0; j<M; j++){
            A[i][j] = rand();
            B[j][i] = rand();
        }
    memset(C, 0, N*N*sizeof(int));
    memset(D, 0, N*N*sizeof(int));
}

void *multi_thread(void *vargp){
    printf("thread begin\n");
    const int min_n = *(int *)vargp;
    const int max_n = min_n + N_PRE_THREAD;
    for(int i=min_n; i<max_n; i++){
        for(int j=0; j<M; j++){
            const elementtype a = A[i][j];
            for(int k=0; k<N; k++){
                C[i][k]+= a*B[j][k];
            }
        }
    }
    printf("thread finish\n");
}

void multi_concurrent(void){
    printf("concurrent begin\n");
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            const elementtype a = A[i][j];
            for(int k=0; k<N; k++){
                D[i][k]+= a*B[j][k];
            }
        }
    }
    printf("concurrent end\n");
}

int main(void){
    int min_ns[THREAD_NUM];
    pthread_t tids[THREAD_NUM];
    
    init_martix();
    for(int i=0; i<THREAD_NUM; i++){
        min_ns[i] = i * N_PRE_THREAD;
        Pthread_create(&tids[i], NULL, multi_thread, &min_ns[i]);
    }

    multi_concurrent();

    for(int i=0; i<THREAD_NUM; i++)
        Pthread_join(tids[i], NULL);
    
    //check
    for(int i=0; i<N; i++)
        for (int j = 0; j < N; j++)
            if(C[i][j] != D[i][j]){
                printf("WRONG!\n");
                exit(0);
            }
    printf("right!\n");
    exit(0);
}