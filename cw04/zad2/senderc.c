#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int counter=0;
void signal_handler(int sig,siginfo_t * siginfo_t1,void*v){
    if(sig==SIGRTMAX-2) {
        counter++;
    }else if(sig==SIGRTMAX-1) {
        printf("Odebrano %d sygnalow\n", counter);
        exit(0);
    }
    return;
}
int main(int argc, char * argv[]) {
    if(argc!=2){
        printf("Zle argumenty\n");
        return -1;
    }
    int n = atoi(argv[1]);
    if(n==0){
        printf("Podano nieprawidłowoą wartość argumentu\n");
        return -1;
    }
    FILE * f = popen("pidof catcherc","r");
    if(f==NULL){
        printf("Nie udało się znaleźć programu catcher\n");
        return -1;
    }
    int pid;
    int flag= fscanf(f,"%d",&pid);
    pclose(f);
    if(flag==0){
        printf("Pid nieodczytany\n");
        return -1;
    }
    union sigval sigval1;
    struct sigaction sigaction1;
    sigaction1.sa_sigaction=signal_handler;
    sigaction1.sa_flags=SA_SIGINFO;
    sigset_t sigset_t1;
    sigfillset(&sigset_t1);
    sigaction1.sa_mask=sigset_t1;
    sigaction(SIGRTMAX-2,&sigaction1,NULL);
    sigaction(SIGRTMAX-1,&sigaction1,NULL);
    for(int i = 0; i<n;i++) {
        if (sigqueue(pid, SIGRTMAX-2, sigval1)) {
            printf("Nie udało się wysłać sygnału\n");
            return -1;
        }
    }
    if (sigqueue(pid, SIGRTMAX-1, sigval1)) {
        printf("Nie udało się wysłać sygnału zakończenia\n");
        return -1;
    }

    while(1)
        pause();
}