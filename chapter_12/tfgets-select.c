#include "sys/select.h"
#include <stdio.h>
#include <unistd.h>

char *tfgets(char *s, int size, FILE *stream){
    fd_set mask;
    FD_ZERO(&mask);
    FD_SET(STDIN_FILENO, &mask);
    struct timeval time_limit;
    time_limit.tv_sec = 5;
    time_limit.tv_usec = 0;
    select(STDIN_FILENO+1, &mask, NULL, NULL, &time_limit);
    if(FD_ISSET(STDIN_FILENO, &mask))
        return fgets(s, size, stream);
    else
        return NULL;
}