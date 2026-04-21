#define _GNU_SOURCE
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection_hdr = "Connection: close\r\n";
static const char *proxy_connection_hdr = "Proxy-Connection: close\r\n";

void doit(int fd);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
void parse_uri(char *uri, char *host, char *port, char *path);
void splice_forward(int src_fd, int dst_fd);

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
        const int client_fd = Accept(listenfd, (SA*)&client_con, &client_len);

        char host[MAXLINE], port[MAXLINE];
        Getnameinfo((SA*)&client_con, client_len, host, sizeof host, port, sizeof port, 0);
        printf("Accepted connection from (%s, %s)\n", host, port);
        doit(client_fd);
        Close(client_fd);
    }

    printf("%s", user_agent_hdr);
    return 0;
}

void doit(int client_fd){
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio, client_fd);

    //读取与解析请求头
    if(!Rio_readlineb(&rio, buf, MAXLINE))
        return;
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")){
        clienterror(client_fd, method, "501", "Not Implemented", 
            "proxy does not implement this method");
        return;
    }

    //解析uri
    char host[MAXLINE], path[MAXLINE], port[6];
    parse_uri(uri, host, port, path);

    sprintf(buf, "GET %s  HTTP/1.0\r\n%s%s%s\r\n", path, 
        user_agent_hdr, connection_hdr, proxy_connection_hdr);

    //连接服务器
    int server_fd = Open_clientfd(host, port);
    if(server_fd < 0){
        clienterror(client_fd, "network", "404", "connection refused", 
            "proxy cannot connect to host");
        return;
    }

    //转发给服务器
    rio_t server;
    Rio_readinitb(&server, server_fd);
    Rio_writen(server_fd, buf, strlen(buf));

    //回复给客户端
    splice_forward(server_fd, client_fd);

    Close(server_fd);
    return;
}

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */

/*
 * parse_uri - parse URI into host and port and path args
 */
/* $begin parse_uri */
void parse_uri(char *uri, char *restrict host, char *restrict port, char *restrict path) 
{
    char *host_ptr = strstr(uri, "//");

    if(host_ptr == NULL){
        char* path_ptr = strchr(uri, '/');
        path = strncpy(path, path_ptr, MAXLINE);
        *port = 80;
        return;
    }

    char *port_ptr = strchr(host_ptr + 2, ':');
    if(port_ptr != NULL){
        short port_num;
        sscanf(port_ptr+1, "%hd%s", &port_num, path);
        sprintf(port, "%hd", port_num);
        *port_ptr = '\0';
    }else{
        char *path_ptr = strchr(host_ptr + 2, '/');
        *port = 80;
        strncpy(path, path_ptr, MAXLINE);
        *path_ptr = '\0';
    }
    strncpy(host, host_ptr+2, MAXLINE);
    return;
}
/* $end parse_uri */

/**
 * splice_forward - zero-cpoy forward
 */
/* $begin splice_forward */
void splice_forward(int src_fd, int dst_fd) {
    int pipe_fds[2];
    if (pipe(pipe_fds) < 0) {
        perror("pipe");
        return;
    }
    
    ssize_t n;
    while ((n = splice(src_fd, NULL, pipe_fds[1], NULL, 8192, 
                       SPLICE_F_MOVE | SPLICE_F_MORE)) > 0) {
        if (splice(pipe_fds[0], NULL, dst_fd, NULL, n, 
                   SPLICE_F_MOVE | SPLICE_F_MORE) != n) {
            break;
        }
    }
    
    close(pipe_fds[0]);
    close(pipe_fds[1]);
}
/* $end splice_forward */
