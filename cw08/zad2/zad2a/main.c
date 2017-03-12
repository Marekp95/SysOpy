#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

pthread_t * pthread_t1;
int start_flag = 0;
int threads_number=10;

void handler (int signo, siginfo_t * info, void * v) {
    printf ("sygnał: %d, PID: %ld, TID: %u\n", info->si_signo, (long)info->si_pid, (unsigned) pthread_self());
}

void * fun(void *v) {
#ifdef PKT_3
    struct sigaction sigaction1;
    sigaction1.sa_flags=SA_SIGINFO;
    sigaction1.sa_sigaction=handler;
    sigaction(SIGUSR1,&sigaction1,NULL);
    sigaction(SIGTERM,&sigaction1,NULL);
    sigaction(SIGKILL,&sigaction1,NULL);
    sigaction(SIGSTOP,&sigaction1,NULL);
#endif
#ifdef PKT_4
    sigset_t sigset_t1;
    sigaddset(&sigset_t1,SIGKILL);
    sigaddset(&sigset_t1,SIGUSR1);
    sigaddset(&sigset_t1,SIGSTOP);
    sigaddset(&sigset_t1,SIGTERM);
    sigprocmask(SIG_SETMASK,&sigset_t1,NULL);
#endif
#ifdef PKT_5
    struct sigaction sigaction1;
    sigaction1.sa_flags=SA_SIGINFO;
    sigaction1.sa_sigaction=handler;
    sigaction(SIGUSR1,&sigaction1,NULL);
    sigaction(SIGTERM,&sigaction1,NULL);
    sigaction(SIGKILL,&sigaction1,NULL);
    sigaction(SIGSTOP,&sigaction1,NULL);
#endif

    while (start_flag == 0);

    while(1){
        printf("%lu\n",pthread_self());
        sleep(1);
    }

    return NULL;
}

int main(int argc, char * argv[]) {
    if(argc!=2){
        printf("Podaj numer sygnału\n");
        return -1;
    }
    int signal;
    switch (argv[1][0]){
        case '1': printf("SIGKILL\n");
            signal=SIGKILL;
            break;
        case '2': printf("SIGSTOP\n");
            signal=SIGSTOP;
            break;
        case '3': printf("SIGUSR1\n");
            signal=SIGUSR1;
            break;
        case '4': printf("SIGTERM\n");
            signal=SIGTERM;
            break;
        default:
            printf("Błędny argument\n");
            return -1;
    }

    pthread_t1 = (pthread_t *) malloc(threads_number * sizeof(pthread_t));

#ifdef PKT_2
    sigset_t sigset_t1;
    sigaddset(&sigset_t1,SIGKILL);
    sigaddset(&sigset_t1,SIGUSR1);
    sigaddset(&sigset_t1,SIGSTOP);
    sigaddset(&sigset_t1,SIGTERM);
    sigprocmask(SIG_SETMASK,&sigset_t1,NULL);
#endif
#ifdef PKT_3
    struct sigaction sigaction1;
    sigaction1.sa_flags=SA_SIGINFO;
    sigaction1.sa_sigaction=handler;
    sigaction(SIGUSR1,&sigaction1,NULL);
    sigaction(SIGTERM,&sigaction1,NULL);
    sigaction(SIGKILL,&sigaction1,NULL);
    sigaction(SIGSTOP,&sigaction1,NULL);
#endif

    for (int i = 0; i < threads_number; ++i) {
        if (pthread_create(&pthread_t1[i], NULL, fun, NULL) != 0) {
            printf("Error 001\n");
        }
    }

    start_flag = 1;
#ifndef PKT_4
#ifndef PKT_5
#define AAA
    kill(getpid(),signal);
#endif
#endif
    sleep(1);
#ifndef AAA
    pthread_kill(pthread_t1[0],signal);
#endif

    for (int i = 0; i < threads_number; ++i) {
        if (pthread_join(pthread_t1[i], NULL) != 0) {
            printf("Error 002\n");
        }
    }

    free(pthread_t1);
    return 0;
}