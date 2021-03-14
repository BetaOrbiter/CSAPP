#include "../csapp.h"

jmp_buf env;

void sigchld_handler(int signum){
    siglongjmp(env, 1);
}
char *tfgets(char *s, int size, FILE *stream){
    if(0 == Fork()){
        Sleep(5);
        exit(0);
    }

    switch(sigsetjmp(env, 0)){
        case 0:
            Signal(SIGCHLD, sigchld_handler);
            return fgets(s, size, stream);
        case 1:
            return NULL;
    }
}