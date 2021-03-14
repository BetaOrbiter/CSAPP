#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr,"require argument!\n");
        exit(EXIT_FAILURE);
    }

    int ip;
    int a,b,c,d;
    sscanf(argv[1], "%x", &ip);
    a = ((unsigned)ip >> 24);
    b = ((ip & 0x00ff0000) >> 16);
    c = ((ip & 0x0000ff00) >> 8);
    d = ip & 0x000000ff;
    printf("%d.%d.%d.%d\n", a, b, c, d);

    return 0;
}