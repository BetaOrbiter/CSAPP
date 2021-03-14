#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#define TIME 5
#define SIZE 80
jmp_buf buf;

void handle(int sigNum){
    if(SIGALRM == sigNum){
        longjmp(buf,1);
    }
}
char *tfgets(char *s, int size, FILE *stream){
    signal(SIGALRM,handle);
    alarm(TIME);
    if(0 == setjmp(buf))
        return fgets(s, size, stream);
    else
        return NULL;
}
int main(void){
    char line[SIZE];
    if(NULL != tfgets(line, SIZE, stdin))
        printf("The result is:%s\n", line);
    else
        printf("Time out!\n");

    return 0;
}