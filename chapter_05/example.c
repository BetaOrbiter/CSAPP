//例程
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#define SIZE 100000000
void setrow(double *a,double *b,long i,long n){
    long j;
    for(j=0;j<n;++j)
        a[n*i+j]=b[j];
}
void setrow2(double *a,double *b,long i,long n){
    long j;
    long ni=n*i;
    for(j=0;j<n;++j)
        a[ni+j]=b[j];
}
void sum_rows1(double *a,double *b, long n){
    for(long i =0;i<n;++i){
        b[i]=0;
        for(long j=0;j<n;++j)
            b[i]+=a[i*n+j];
    }
}
void sum_rows2(double *a,double *b,long n){
    for(long i=0,ni=0;i<n;++i,ni+=n){
        double sum=0;
        for(int j=0;j<n;++j)
            sum+=a[ni+j];
        b[i]=sum;
    }
}

double poly(double a[], double x, long long degree){
    long long i;
    double result = a[0];
    double xpwr = x;
    for(i = 1; i<=degree; i++){
        result += a[i]*xpwr;
        xpwr = x* xpwr;
    }
    return result;
}
double polyh(double a[], double x, long long degree){
    long long i;
    double result = a[0];
    double xpwr = x;
    for(i = 1; i<=degree; i++){
        result = a[i]+x*result;
    }
    return result;
}
int main(void){
    double *b = (void *)malloc(SIZE * sizeof(double));
    double *a = (void *)malloc(SIZE * sizeof(double));
    
    double x = 5;
    double o,oo;
    clock_t t1 = clock();
    o=poly(a,x,SIZE);
    clock_t t2 = clock();
    printf("ploy cost: %ld\nget %f\n",t2-t1,o);

    clock_t t3 = clock();
    oo=polyh(b,x,SIZE);
    clock_t t4 = clock();
    printf("ployh cost: %ld\nget %f\n",t4-t3,oo);
    
    return 0;
}
void merge(long long src1[],long long src2[],long long dest[],const long long n){
    long long i1 =0;
    long long i2 =0;
    long long id =0;
    while(i1<n&&i2<n){
        long long v1 = src1[i1];
        long long v2 = src2[i2];
        dest[id++] = v1<v2?v1:v2;
        i1 += v1<v2;
        i2 += !(v1<v2);
        // if(src1[i1]<src2[i2])
            // dest[id++] = src1[i1++];
        // else
            // dest[id++] = src2[i2++];
    }
    while (i1<n)
        dest[id++] = src1[i1++];
    while (i2<n)
        dest[id++] = src1[i2++];
}