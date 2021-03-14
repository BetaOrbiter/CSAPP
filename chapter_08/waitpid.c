#include "../csapp.h"
#include "stdio.h"
#define N 2

int main(void){
    int status, i;
    pid_t pid;

    for(i=0; i<N; i++)
        if(0 == (pid = Fork())){
            int *p = NULL;
            *p = 1;
            exit(i+100);
        }
    
    while (0 < (pid = waitpid(-1, &status, 0)))
    {
        if(WIFEXITED(status))
            printf("child %d terminated normally with exit status=%d\n",
                    pid,WEXITSTATUS(status));
        else
            if(WIFSIGNALED(status))
            {
                char line[100];
                sprintf(line, "child %d terminated by signal %d", pid, WTERMSIG(status));
                psignal(WTERMSIG(status),line);
            }
    }
    if(errno != ECHILD)
        unix_error("waitpid error");
    
    exit(0);
}