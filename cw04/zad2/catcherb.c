#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void signal_handler(int sig,siginfo_t * siginfo_t1,void*v){
    union sigval sigval1;
    switch (sig){
        case SIGUSR1:
            if (sigqueue(siginfo_t1->si_pid, SIGUSR1, sigval1)) {
                printf("Nie udało się wysłać sygnału\n");
            }
            break;
        case SIGUSR2:

            if (sigqueue(siginfo_t1->si_pid, SIGUSR2, sigval1)) {
                printf("Nie udało się wysłać sygnału zakończenia\n");
                return;
            }
            exit(0);
        default:
            break;
    }
    return;
}
int main() {
    struct sigaction sigaction1;
    sigaction1.sa_sigaction=signal_handler;
    sigaction1.sa_flags=SA_SIGINFO;
    sigset_t sigset_t1;
    sigfillset(&sigset_t1);
    sigaction1.sa_mask=sigset_t1;
    sigaction(SIGUSR1,&sigaction1,NULL);
    sigaction(SIGUSR2,&sigaction1,NULL);
    while(1)
        pause();
}