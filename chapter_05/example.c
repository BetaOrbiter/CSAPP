//例程
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