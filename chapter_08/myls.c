#include<unistd.h>
#include<stdio.h>
#include<errno.h>
int main(int argc, char *argv[], char *envp[]){
    if(-1 == execve("/bin/ls",argv,envp))
        perror("failed!");
    return 0;
}