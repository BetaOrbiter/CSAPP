#include "../csapp.h"

int main(int argc, char *argv[]){
    struct addrinfo *p, *list, hints;
    char buf[MAXLINE];
    int rc, flags;

    if(2 != argc){
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    //Get a list of addrinfo records
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      //IPV4 only
    hints.ai_socktype = SOCK_STREAM;// Connections only
    if(0 != (rc = getaddrinfo(argv[1],NULL, &hints, &list))){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    //Walk the list and display each IP address
    flags = NI_NUMERICHOST;//Display address string instead of domain name
    for(p = list; p; p = p->ai_next){
        getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
        printf("%s\n", buf);
    }

    freeaddrinfo(list);
    return 0;
}