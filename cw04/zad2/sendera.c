#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
int counter=0;
void signal_handler(int sig,siginfo_t * siginfo_t1,void*v){
    switch (sig){
        case SIGUSR1:
            counter++;
            break;
        case SIGUSR2:
            printf("Odebrano %d sygnalow\n",counter);
            exit(0);
        default:
            break;
    }
    return;
}
int main(int argc, char * argv[]) {
    if(argc!=2){
        printf("Zle argumenty\n");
        return 0;
    }
    int n = atoi(argv[1]);
    if(n==0){
        printf("Podano nieprawidłowoą wartość argumentu\n");
        return 0;
    }
    FILE * f = popen("pidof catchera","r");
    if(f==NULL){
        printf("Nie udało się znaleźć programu catcher\n");
        return 0;
    }
    int pid;
    int flag= fscanf(f,"%d",&pid);
    pclose(f);
    if(flag==0){
        printf("Pid nieodczytany\n");
        return 0;
    }
    union sigval sigval1;
    struct sigaction sigaction1;
    sigaction1.sa_sigaction=signal_handler;
    sigaction1.sa_flags=SA_SIGINFO;
    sigset_t sigset_t1;
    sigfillset(&sigset_t1);
    sigaction1.sa_mask=sigset_t1;
    sigaction(SIGUSR1,&sigaction1,NULL);
    sigaction(SIGUSR2,&sigaction1,NULL);
    for(int i = 0; i<n;i++) {
        if (sigqueue(pid, SIGUSR1, sigval1)) {
            printf("Nie udało się wysłać sygnału\n");
            return 0;
        }
    }
    if (sigqueue(pid, SIGUSR2, sigval1)) {
        printf("Nie udało się wysłać sygnału zakończenia\n");
        return 0;
    }
    
    while(1)
        pause();
}