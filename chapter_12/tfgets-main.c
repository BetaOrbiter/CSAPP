#include "../csapp.h"
#include "tfgets-select.c"
int main(void){
    char buf[MAXLINE];

    if(tfgets(buf, MAXLINE, stdin) == NULL)
        printf("BOOM!\n");
    else
        printf("%s", buf);
    exit(0);
}