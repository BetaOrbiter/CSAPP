long long rsum(long long *start, long long count){
    if(count<=0)
        return 0;
    return *start + rsum(start+1,count-1);
}