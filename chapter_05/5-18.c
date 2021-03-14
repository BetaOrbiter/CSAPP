#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000000
typedef long ElementType;
ElementType poly(ElementType a[], ElementType x, long degree){
    long long i;
    ElementType result = a[0];
    ElementType xpwr = x;
    for(i = 1; i<=degree; i++){
        result += a[i]*xpwr;
        xpwr = x* xpwr;
    }
    return result;
}

ElementType poly_3x2(const ElementType a[],const ElementType x,const long degree){
    long long i;
    ElementType result0 = 0;
    ElementType result1 = 0;
    ElementType result2 = 0;
    
    ElementType xpwr0 = 1;
    ElementType xpwr1 = x*x;
    ElementType xpwr2 = x*x*x*x;

    const ElementType dxpwr = x*x*x*x*x*x;

    for(i = 0; i<=degree-6; i+=6){
        result0 =result0 + (xpwr0*(a[i] + a[i+1]*x));
        result1 =result1 + (xpwr1*(a[i+2] + a[i+3]*x));
        result2 =result2 + (xpwr2*(a[i+4] + a[i+5]*x));

        xpwr0 *= dxpwr;
        xpwr1 *= dxpwr;
        xpwr2 *= dxpwr;
    }
    for(;i<=degree;++i,xpwr0*=x)
        result0 += a[i]*xpwr0;
    return result0 + result1 + result2;
}
int main(void){
    ElementType x = 5;
    ElementType oo;
    clock_t t1,t2;

    ElementType *a = (void *)malloc(SIZE * sizeof(ElementType));
    t1 = clock();
    oo=poly(a,x,SIZE);
    t2 = clock();
    free(a);
    printf("ploy cost: %ld\nget %ld\n",t2-t1,oo);

    ElementType *c = (void *)malloc(SIZE * sizeof(ElementType));
    t1 = clock();
    oo=poly_3x2(c,x,SIZE);
    t2 = clock();
    free(c);
    printf("ploy_8x8 cost: %ld\nget %ld\n",t2-t1,oo);
        
    return 0;
}