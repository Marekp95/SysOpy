#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

int start_flag = 0;

void handler (int signo, siginfo_t * info, void * v) {
    printf("Sygnał: %d, PID: %ld, TID: %u\n", info->si_signo, (long)info->si_pid, (unsigned) pthread_self());
}

void * fun(void *v) {

    struct sigaction sigaction1;
    sigaction1.sa_flags=SA_SIGINFO;
    sigaction1.sa_sigaction=handler;
    sigaction(SIGFPE,&sigaction1,NULL);

    while (start_flag == 0);
    printf("%d\n",1/0);

    return NULL;
}
void * fun2(void *v) {

    struct sigaction sigaction1;
    sigaction1.sa_flags=SA_SIGINFO;
    sigaction1.sa_sigaction=handler;
    sigaction(SIGFPE,&sigaction1,NULL);

    while (start_flag == 0);
    while(1)
    printf("dziala\n");

    return NULL;
}

int main(int argc, char * argv[]) {

    pthread_t pthread_t2;
    pthread_t pthread_t3;
    pthread_create(&pthread_t2, NULL, fun, NULL);
    pthread_create(&pthread_t3, NULL, fun2, NULL);

    start_flag = 1;

    pthread_join(pthread_t3, NULL);
    pthread_join(pthread_t2, NULL);

    return 0;
}