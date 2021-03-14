//第六章矩阵乘法，验证局部性
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE (1<<10)
#define BLOCK_SIZE 64

int a1[SIZE][SIZE];
int b1[SIZE][SIZE];
int c1[SIZE][SIZE];

int a2[SIZE][SIZE];
int b2[SIZE][SIZE];
int c2[SIZE][SIZE];

void mul_plain(void){
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++){
            int m=0;
            for(int k=0;k<SIZE;k++)
                m += a1[i][k]*b1[k][j];
            c1[i][j] = m;
        }
}

void mul_fast(void){
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++){
            const int m = a2[i][j];
            for(int k=0;k<SIZE;k++)
                c2[i][k] += b2[j][k]*m;
        }
}
void mul_block(void){
    for(int i=0;i<SIZE;i+=BLOCK_SIZE){
        for(int j=0;j<SIZE;j+=BLOCK_SIZE){
            for(int k=0;k<SIZE;k+=BLOCK_SIZE){
                for(int i1=i;i1<i+BLOCK_SIZE;i1++){
                    for(int j1=j;j1<j+BLOCK_SIZE;j1++){
                        int sum = 0;
                        for(int k1=k;k1<k+BLOCK_SIZE;k1++)
                            sum+=a1[i1][k1]*b1[k1][j1];
                        c1[i1][j1]+=sum;
                    }
                }
            }
        }
    }
}
int main(void){
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++){
            a2[i][j] = a1[i][j] = rand()&1;
            b2[i][j] = b1[i][j] = rand()&1;
            c2[i][j] = c1[i][j] = 0;
            
        }
    puts("set finished");

    clock_t t1,t2;

    t1 = clock();
    mul_block();
    t2 = clock();
    printf("block costs:%5ld\n",t2-t1);

    t1=clock();
    mul_fast();
    t2=clock();
    printf("better costs:%5ld\n", t2-t1);

    int flg=1;
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++)
            if(c1[i][j]!=c2[i][j]){
                flg = 0;
                break;
            }
        if(0==flg)
            break;
    }
    puts(flg?"yes":"fuck");
    return 0;
}