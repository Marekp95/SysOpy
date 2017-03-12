#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int counter;
void signal_handler(int sig,siginfo_t * siginfo_t1,void*v){
    union sigval sigval1;
    if(sig==SIGRTMAX-2) {
        counter++;
    }else if(sig==SIGRTMAX-1) {
        for (int i = 0; i < counter; i++) {
            if (sigqueue(siginfo_t1->si_pid, SIGRTMAX-2, sigval1)) {
                printf("Nie udało się wysłać sygnału\n");
                return;
            }
        }
        if (sigqueue(siginfo_t1->si_pid, SIGRTMAX-1, sigval1)) {
            printf("Nie udało się wysłać sygnału zakończenia\n");
            return;
        }
        exit(0);
    }
    return;
}
int main() {
    counter=0;
    struct sigaction sigaction1;
    sigaction1.sa_sigaction=signal_handler;
    sigaction1.sa_flags=SA_SIGINFO;
    sigset_t sigset_t1;
    sigfillset(&sigset_t1);
    sigaction1.sa_mask=sigset_t1;
    sigaction(SIGRTMAX-2,&sigaction1,NULL);
    sigaction(SIGRTMAX-1,&sigaction1,NULL);
    while(1)
        pause();
}