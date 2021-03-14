//本程序根据命令行参数输出m*n二维int矩阵缓存映射关系
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main(int argc, char **argv){
    const int m = atoi(argv[1]),n=atoi(argv[2]);
    const uint64_t ci=((1<<5)-1)<<5;
    const uint64_t co=(1<<5)-1;
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            uint64_t index = ((i*n+j)<<2)&ci;
            printf("%5lu",index>>5);
        }
        putchar('\n');
    }
    return 0;
}