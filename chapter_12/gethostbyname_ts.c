#include "../csapp.h"

static sem_t mutex;
struct hostent *gethostbyname_ts(const char*name){
    P(&mutex);

    struct hostent *host = gethostbyname(name);
    struct hostent *result;

    result->h_addrtype = host->h_addrtype;
    result->h_length = host->h_length;

    result->h_name = (char*)Malloc(strlen(host->h_name)+1);
    strcpy(result->h_name, host->h_name);
    Free(host->h_name);

    int i;
    for(i=0; host->h_addr_list[i]!=NULL; i++)
        continue;
    result->h_addr_list = Malloc(sizeof(char*)*(i+1));
    for(int j=0;j<i;j++){
        result->h_addr_list[j] = Malloc(1 + strlen(host->h_addr_list[j]));
        strcpy(result->h_addr_list[j], host->h_addr_list[j]);
        Free(host->h_addr_list[j]);
    }
    result->h_addr_list[i] = NULL;
    Free(host->h_addr_list);

    for(i=0; host->h_aliases[i]!=NULL; i++)
        continue;
    result->h_aliases = Malloc(sizeof(char*) * (i+1));
    for(int j=0; j<i; j++){
        result->h_aliases[j] = Malloc(1 + strlen(host->h_aliases[j]));
        strcpy(result->h_aliases[j], host->h_aliases[j]);
        Free(host->h_aliases[j]);
    }
    result->h_aliases[i] = NULL;
    Free(host->h_aliases);

    Free(host);
    
    V(&mutex);
    return result;
}