#include "../csapp.h"
void echo(int connfd);
void command(void);
void *comunicate(void *fdp);

int main(int argc, char **argv){
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fd_set read_set, ready_set;

    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n",argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(atoi(argv[1]));

    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(listenfd, &read_set);

    while (1){
        ready_set = read_set;
        Select(listenfd+1, &ready_set, NULL, NULL, NULL);
        if(FD_ISSET(STDIN_FILENO, &ready_set))
            command();
        if(FD_ISSET(listenfd, &ready_set)){
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
            
            int *fdp = malloc(sizeof(int));
            *fdp = connfd;
            Pthread_create(NULL, NULL, comunicate, NULL);
        }
    }
    
}

void *comunicate(void *fdp){
    Pthread_detach(Pthread_self());
    int connfd = *(int*)fdp;
    free(fdp);
    echo(connfd);
    Close(connfd);
}

void command(void){
    char buf[MAXLINE];
    if(!Fgets(buf, MAXLINE, stdin))
        exit(0);
    printf("%s", buf);
}
void echo(int connfd){
    
    rio_t t;
    char buf[MAXLINE];
    Rio_readinitb(&t, connfd);
    Rio_readlineb(&t,buf,MAXLINE);
    Rio_writen(connfd, buf, strlen(buf)-1);
}