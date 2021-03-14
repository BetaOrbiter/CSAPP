#include<stdio.h>
#include<stdlib.h>
int main(int argc, char *argv[], char *envp[]){
    int i;
    
    while(1)
        ;

    printf("Command-line arguments:\n");

    for(int i=0; argv[i]!=NULL; i++)
        printf("    argv[%2d]: %s\n", i, argv[i]);

    putchar('\n');
    printf("Enviroment variables:\n");
    for (int i = 0; envp[i] != NULL; i++)
        printf("     envp[%2d]: %s\n", i, envp[i]);
    
    exit(0);
}