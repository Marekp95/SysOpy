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

int main(int argc, char * argv[]) {
    if(argc<2){
        printf("Błędne argumenty\n");
        return -1;
    }
    int fifo = open(argv[1],O_WRONLY);
    if(fifo==-1){
        printf("Error\n");
        return -1;
    }
    while(1){
        int size = PIPE_BUF - sizeof(getpid()) - sizeof(time_t) - 10;
        char bufor[size];
        fgets(bufor,size,stdin);
        time_t time_t1 = time(NULL);
        char message[sizeof(getpid())+ strlen(bufor)+ sizeof(time_t)+10];
        sprintf(message," - %d - %ld - %s",getpid(),time_t1,bufor);
        write(fifo,message, strlen(message)+1);
    }
}