#include "../csapp.h"

int main(int argc, char ** argv)
{
    int n, fd;
    rio_t rio;
    char buf[MAXLINE];

    if(2 == argc){
        fd = Open(argv[1], O_RDONLY, S_IREAD);
        Dup2(fd, STDIN_FILENO);
        Close(fd);
    }
    Rio_readinitb(&rio, STDIN_FILENO);
    while ((n = Rio_readnb(&rio, buf, 10)) != 0)
        Rio_writen(STDOUT_FILENO, buf, 10);    
}