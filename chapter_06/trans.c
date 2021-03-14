//第六章矩阵乘法，验证局部性
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define SIZE (1<<11)

int a1[SIZE][SIZE];
int b1[SIZE][SIZE];
// int c1[SIZE][SIZE];

int a2[SIZE][SIZE];
int b2[SIZE][SIZE];
// int c2[SIZE][SIZE];

void trans1(void){
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++){
            b1[i][j] = a1[i][j];
        }
}
void trans2(void){
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++){
            int m = a2[i][j];
            for(int k=0;k<SIZE;k++)
                c2[i][k] += b2[j][k]*m;
        }
}
int main(void){
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++){
            a2[i][j] = a1[i][j] = rand()&1;
            b2[i][j] = b1[i][j] = rand()&1;
            // c2[i][j] = c1[i][j] = 0;            
        }
    puts("set finished");

    clock_t t1,t2;

    t1 = clock();
    trans1();
    t2 = clock();
    printf("plain costs:%5ld\n",t2-t1);

    t1=clock();
    mul2();
    t2=clock();
    printf("better costs:%5ld\n", t2-t1);

    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++)
            if(c1[i][j]!=c2[i][j]){
                puts("fuck!");
                break;
            }
    puts("yes");
    return 0;
}