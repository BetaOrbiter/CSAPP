#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int mysystem(char * command){
    pid_t pid;
    int status;
    if(0 == (pid=fork())){
        execl("/bin/sh","sh","-c",command,(char*)NULL);
    }
    else
    {
        if(pid == waitpid(pid,&status,0)){
            if(WIFEXITED(status))
                return WEXITSTATUS(status);
            else
                if(WIFSIGNALED(status)){
                    fprintf(stderr,"command terminated by signal No.%d.\n",WTERMSIG(status));
                    return WTERMSIG(status);
                }
        }
        else{
            perror("fail to reap /bin/sh.\n");
            return EXIT_FAILURE;
        }
    }
}
int main(int argc, char *argv[]){
    if(argc > 1)
        mysystem(argv[1]);
    else
        perror("Oh, shit!\n");
    return 0;
}