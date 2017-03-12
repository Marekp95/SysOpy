#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/time.h>
#include <string.h>
#include "zad2.h"

sem_t *sem_t1;
int shm1;
struct memory * memory1 = (struct memory *) -1;
int u = 0;
int n = 0;
int x;

void clean() {
    if (sem_t1 >= 0) {
        sem_close(sem_t1);
    }
    if (shm1 >= 0) {
        if (memory1 >= 0) {
            munmap(memory1, STRUCT_SIZE);
        }
        close(shm1);
    }
}

void handler(int sig) {
    if (u) {
        printf("%d Znalazlem %d liczb: %d.\n", getpid(), n, x);
    }
    exit(0);
}


int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);

    if (argc != 2 && argc != 3) {
        printf("Podano błędne argumenty\n");
        exit(-1);
    }

    if (argc == 3) {
        if (strcmp(argv[2], "-u") == 0) {
            u = 1;
        }
        else {
            printf("Error 001\n");
            exit(-1);
        }
    }
    x = atoi(argv[1]);

    srand(7);

    shm1 = shm_open("/shm", O_RDWR, 0);
    if (shm1 < 0 || (memory1 = (struct memory *) mmap(0, STRUCT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm1, 0)) < 0) {
        printf("Error 003\n");
        exit(-1);
    }

    sem_t1 = sem_open("/sem_t1", 0);
    if (sem_t1 < 0) {
        printf("Error 004\n");
        exit(-1);
    }

    int counter;
    struct timeval timeval1;
    while (1) {
        counter = 0;
        if (sem_wait(sem_t1) < 0) {
            printf("Error 005\n");
            exit(-1);
        }
        for (int i = 0; i < SIZE; i++) {
            if (memory1->t[i] == x) {
                counter++;
            }
        }
        n += counter;
        gettimeofday(&timeval1, NULL);
        if (u == 0)
            printf("(%d %ld.%ld) Znalazlem %d liczb: %d.\n", getpid(), timeval1.tv_sec, timeval1.tv_usec / 1000, counter, x);
        if (sem_post(sem_t1) < 0) {
            perror(NULL);
            printf("Error 006\n");
        }
        sleep(1);
    }
}