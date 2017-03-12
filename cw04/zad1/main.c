#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int counter = 1;
int max_num;
int direction = 1;
void signal_handler(int sig){
    switch (sig){
        case SIGINT:
            printf("Odebrano sgnał SIGINT\n");
            exit(0);
        case SIGTSTP:
            if(counter==1)
                direction=1;
            else if(counter==max_num*3)
                direction=-1;
            counter+=direction;
            break;
        default:
            break;
    }
    return;
}
void print_reverse(char * s){
    for(long i = strlen(s);i>0;i--){
        printf("%c",s[i-1]);
    }
    printf("\n");
    return;
}
int main (int argc, char *argv[]) {
    if(argc!=3){
        printf("Zle argumenty\n");
        return 0;
    }
    max_num=atoi(argv[2]);
    if(max_num==0){
        printf("Arument 2 nie jest liczba\n");
        return 0;
    }
    char * txt = argv[1];
    signal(SIGINT, signal_handler);
    struct sigaction sigaction1;
    sigaction1.sa_handler=signal_handler;
    sigaction(SIGTSTP,&sigaction1,NULL);
    while (1) {
        for(int i =0;i<=(counter-1)/3;i++){
            if(counter%3==2)
                print_reverse(txt);
            else
                printf("%s\n",txt);
        }
        printf("\n");
        sleep (1);
    }
}
