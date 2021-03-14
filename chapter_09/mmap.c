#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void){
    int fd;
    struct stat s;
    char * chs;

    fd = open("hello.txt",O_RDWR);
    if(-1 == fd){
        perror( strerror(errno) );
        return 1;
    }
    if(-1 == fstat(fd, &s) ){
        perror( strerror(errno) );
        return 1;
    }
    chs = mmap(NULL, s.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == chs){
        perror( strerror(errno) );
        return 0;
    }
    chs[0] = 'J';
    return 0;
}