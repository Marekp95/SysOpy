#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>


int main(int argc, char * argv[]) {
    if(argc<2){
        printf("Błędne argumenty\n");
        return -1;
    }
    mkfifo(argv[1],S_IRWXU);
    int fifo = open(argv[1],O_RDONLY);
    if(fifo==-1){
        printf("Error\n");
        return -1;
    }
    char buf[PIPE_BUF];
    while(1){
        if(read(fifo,buf,PIPE_BUF)>0){
            printf("%ld%s\n",time(NULL),buf);
        }
    }
}