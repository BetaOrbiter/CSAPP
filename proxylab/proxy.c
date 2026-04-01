#include "csapp.h"
#include <math.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

static void doit(int fd);

int main(int argc, char *argv[])
{
    short listenfd;

    //check command-line args
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
    } 
    listenfd = Open_listenfd(argv[1]);

    while(1){
        struct sockaddr_storage client_con;
        socklen_t client_len = sizeof client_con;
        const int con_fd = Accept(listenfd, (SA*)&client_con, &client_len);

        char host[MAXLINE], port[MAXLINE];
        Getnameinfo((SA*)&client_con, client_len, host, sizeof host, port, sizeof port, 0);
        printf("Accepted connection from (%s, %s)\n", host, port);
        doit(con_fd);
        Close(con_fd);
    }

    printf("%s", user_agent_hdr);
    return 0;
}

static void doit(int fd){
    //todo 
    return;
}