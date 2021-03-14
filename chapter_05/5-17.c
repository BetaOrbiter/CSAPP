#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void *memset_8x1(void * const s,const int c,const size_t n)
{
    const size_t K = sizeof (unsigned long);
    size_t cnt = 0;
    unsigned char *schar = (unsigned char *)s;
    while ((size_t)schar%K && cnt<n){
        *schar++ = (unsigned char)c;
        cnt++;
    }
    
    unsigned long *sul = (unsigned long *)schar;
    unsigned long Kc = c;
    for(size_t i = 1;i<=K;i<<=1)
        Kc |= (Kc << (i * (sizeof(char)<<3)));

    for(;cnt <n-K;cnt+=K)
        *sul++ = Kc;
    
    for(schar = (unsigned char *)sul;cnt < n;cnt++)
        *schar++ = (unsigned char)c;
    return s;
}
int main(void)
{
    int c = 0xff;
    size_t n = 424239;
    void *s1 = (void*) malloc(n*sizeof(long));
    void *s2 = (void*) malloc(n*sizeof(long));
    memset_8x1(s1, c, n);
    // 使用库函数测试
    memset(s2, c, n);
    
    printf("%d",memcmp(s1, s2, n));
    
    free(s1);
    free(s2);
    return 0;
}