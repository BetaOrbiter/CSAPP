#include <sys/types.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void aaa(int signum){
    printf("Oh, you have press crtl+c!\n");
}

int main(int argc, char *argv[]){
    int statues;
    pid_t pid;

    signal(SIGINT,aaa);

    printf("Hello!");
    pid = fork();
    printf("%d\n",!pid);
    if(pid != 0){
        if(waitpid(-1,&statues,0)>0){
            if(WIFEXITED(statues) != 0)
                printf("%d\n",WEXITSTATUS(statues));
        }
    }
    while(1)
        ;
    printf("Bye\n");
    exit(2);
}
unsigned int snooze(unsigned int secs){
    printf("Slept for %u of %u secs", secs-sleep(secs), secs);
}