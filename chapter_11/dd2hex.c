#include <stdio.h>
#include <stdlib.h>

int main(const int argc,const char *argv[]){
    if(argc < 2){
        fprintf(stderr, "require argument!\n");
        exit(EXIT_FAILURE);
    }

    int ip = 0;
    int four[4] = {0, 0, 0, 0};
    const char *ptr = argv[1];
    for(int i=0; i<4; i++){
        while(*ptr!='\0' && *ptr!='.'){
            four[i] = four[i]*10 + *ptr - '0';
            ++ptr;
        }
        four[i] &= 0xff;
        ++ptr;
    }

    ip = (four[0]<<24) | (four[1] << 16) | (four[2] << 8) | (four[3]);

    printf("%#x\n", ip);
}