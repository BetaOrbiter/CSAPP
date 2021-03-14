#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

unsigned int snooze(unsigned int secs){
    printf("Slept for %d of %d secs\n", secs-sleep(secs), secs);
}
void handler(int signum){
    return;
}
int main(int argc, char *argv[]){
    signal(SIGINT,handler);
    sigset_t mask,pre;
    sigemptyset(&mask);
    sigaddset(&mask,SIGINT);
    sigprocmask(SIG_BLOCK,&mask,&pre);
    int secs = atoi(argv[1]);
    snooze(secs);
}