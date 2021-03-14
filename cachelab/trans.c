/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

#define BLOCK1 8
#define BLOCK2 4

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans_32(int M, int N, int A[N][M], int B[M][N]);
void trans_64(int M, int N, int A[N][M], int B[M][N]);
void trans_61(int M, int N, int A[N][M], int B[M][N]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(32==M && 32==N)
        trans_32(M,N,A,B);
    else
        if(64==M && 64==N)
            trans_64(M,N,A,B);
        else
            trans_61(M,N,A,B);
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

char t32_dec[] = "this is 32X32 martix";
void trans_32(int M, int N, int A[N][M], int B[M][N]){
    for (int i = 0; i < N; i+=BLOCK1) {
        for (int j = 0; j < M; j+=BLOCK1) {
            if(i!=j){
                for (int  i1 = i; i1 < i+BLOCK1; i1++){
                    for(int j1=j;j1<j+BLOCK1;j1++){
                        B[j1][i1] = A[i1][j1];
                    }
                }
            }
            else{
                for(int i1=i;i1<i+BLOCK1; i1++){
                    const int a=A[i1][j];
                    const int b=A[i1][j+1];
                    const int c=A[i1][j+2];
                    const int d=A[i1][j+3];
                    const int e=A[i1][j+4];
                    const int f=A[i1][j+5];
                    const int g=A[i1][j+6];
                    const int h=A[i1][j+7];

                    B[j][i1] = a;
                    B[j+1][i1] = b;
                    B[j+2][i1] = c;
                    B[j+3][i1] = d;
                    B[j+4][i1] = e;
                    B[j+5][i1] = f;
                    B[j+6][i1] = g;
                    B[j+7][i1] = h;
                }
            }
        }
    }

}

char t64_dec[] = "this is 64X64 martix";
void trans_64(int M, int N, int A[N][M], int B[M][N]){
    for(int i=0;i<N;i+=BLOCK1){
        for(int j=0;j<M;j+=BLOCK1){
            for(int i1=i;i1<i+BLOCK2;i1++){
                    const int a=A[i1][j];
                    const int b=A[i1][j+1];
                    const int c=A[i1][j+2];
                    const int d=A[i1][j+3];
                    const int e=A[i1][j+4];
                    const int f=A[i1][j+5];
                    const int g=A[i1][j+6];
                    const int h=A[i1][j+7];

                    B[j][i1] = a;
                    B[j+1][i1] = b;
                    B[j+2][i1] = c;
                    B[j+3][i1] = d;
                    B[j][i1+4] = e;
                    B[j+1][i1+4] = f;
                    B[j+2][i1+4] = g;
                    B[j+3][i1+4] = h;
            }
            for(int j1=j;j1<j+BLOCK2;j1++){
                const int a=B[j1][i+4];
                const int b=B[j1][i+5];
                const int c=B[j1][i+6];
                const int d=B[j1][i+7];

                B[j1][i+4] = A[i+4][j1];
                B[j1][i+5] = A[i+5][j1];
                B[j1][i+6] = A[i+6][j1];
                B[j1][i+7] = A[i+7][j1];
                
                B[j1+4][i] = a;
                B[j1+4][i+1] = b;
                B[j1+4][i+2] = c;
                B[j1+4][i+3] = d;
                
                B[j1+4][i+4] = A[i+4][j1+4];
                B[j1+4][i+5] = A[i+5][j1+4];
                B[j1+4][i+6] = A[i+6][j1+4];
                B[j1+4][i+7] = A[i+7][j1+4];
            }
        }
    }
}
void trans_61(int M, int N, int A[N][M], int B[M][N]){
    for(int i=0;i<N;i+=BLOCK1){
        for(int j=0;j<M;j+=BLOCK1){
            for(int i1=i;i1<i+BLOCK1 && i1<N;i1++){
                for(int j1=j;j1<j+BLOCK1 && j1<M;j1++){
                    B[j1][i1] = A[i1][j1];
                }
            }
        }
    }
}
/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc); 
    // registerTransFunction(trans_32, t32_dec);
    registerTransFunction(trans_64,t64_dec);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

