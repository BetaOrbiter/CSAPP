#define ll long long
//3.58
ll decode(ll x,ll y,ll z){
    y-=z;
    x*=y;
    x^=(y^1)?-1:0;
    return x;
}
//3.61
ll cread_alt(ll *xp){
    static ll x=0;//static -O3 优化才开启条件传送
    if(0==xp)
        xp=&x;
    return *xp;
}
ll switch_prob(ll x,ll n){
    ll result=x;
    switch (n)
    {
    case 0X3C:
    case 0x3E:
        result=x<<3;
        break;
    case 0x3F:
        result = x>>3;
        break;
    case 0x40:x*=15;
    case 0x41:x*=x;
    default:result =x+0x4B;
        break;
    }
    return result;
}
//3.70
union ele
{
    struct 
    {
        long *p;
        long y;
    }e1;
    struct 
    {
        long x;
        union ele *next;
    }e2;
};
void proc(union ele *up){
    up->e2.x=*(up->e2.next->e1.p)-up->e1.y;
}
//71
void good_echo(char s[]){
    
}