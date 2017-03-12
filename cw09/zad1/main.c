#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <zconf.h>
#include <bits/signum.h>
#include <signal.h>

pthread_t filozofowie[5];
pthread_mutex_t widelce[5];
sem_t kelner;
volatile int flaga = 1;

void wez_widelec(int fid) {
    pthread_mutex_lock(&widelce[fid]);
}

void odloz_widelec(int fid) {
    pthread_mutex_unlock(&widelce[fid]);
}

void *fun(void *fid) {
    int id = *((int *) fid);
    int lewy = id;
    int prawy = (id + 1) % 5;
    while (flaga) {
        usleep(rand() % 100);
        sem_wait(&kelner);
        printf("Filozof %d czeka na lewy widelecc%d\n", id, lewy);
        wez_widelec(lewy);
        printf("Filozof %d podniósł lewy widelec%d\n", id, lewy);
        printf("Filozof %d czeka na prawy widelec%d\n", id, prawy);
        wez_widelec(prawy);
        printf("Filozof %d podniósł prawy widelec%d\n", id, prawy);
        printf("Filozof %d je\n", id);
        usleep(rand() % 100);
        printf("Filozof %d odłożył lewy widelec%d\n", id, lewy);
        odloz_widelec(lewy);
        printf("Filozof %d odłożył prawy widelec%d\n", id, prawy);
        odloz_widelec(prawy);
        sem_post(&kelner);
    }
    return NULL;
}

void clean() {
    flaga = 0;
    for (int i = 0; i < 5; i++) {
        if (pthread_join(filozofowie[i], NULL) != 0) {
            printf("Error 006\n");
            exit(6);
        }
    }
    sem_destroy(&kelner);
}

void sighandler(int sig) {
    exit(0);
}

int main(int argc, char *argv[]){

    srand(time(NULL));
    atexit(clean);
    signal(SIGINT, sighandler);
    sem_init(&kelner, 0, 4);

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        printf("Error 001\n");
        exit(1);
    }
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL) != 0) {
        printf("Error 002\n");
        exit(2);
    }
    int pid[5];
    for (int i = 0; i < 5; i++) {
        pid[i] = i;
        if (pthread_mutex_init(&widelce[i], &attr) != 0) {
            printf("Error 003\n");
            exit(3);
        }
    }

    for (int i = 0; i < 5; i++) {
        if (pthread_create(&filozofowie[i], NULL, fun, &pid[i]) != 0) {
            printf("Error 004\n");
            exit(4);
        }
    }

    while (1) {
        pause();
    }
}