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
#include "zad2.h"

sem_t *sem_t1;
sem_t *sem_t2;
int shm1;
struct memory *memory1 = (struct memory *) -1;

void clean() {
    if (sem_t1 >= 0) {
        sem_close(sem_t1);
    }
    if (sem_t2 >= 0) {
        sem_close(sem_t2);
    }
    if (shm1 >= 0) {
        if (memory1 >= 0) {
            munmap(memory1, STRUCT_SIZE);
        }
        close(shm1);
    }
}

void handler(int sig) {
    exit(0);
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);
    srand(7);

    shm1 = shm_open("/shm", O_RDWR, 0);
    if (shm1 < 0) {
        printf("Error 001\n");
        exit(-1);
    }
    memory1 = (struct memory *) mmap(0, STRUCT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm1, 0);
    if (memory1 < 0) {
        perror(NULL);
        exit(-1);
    }

    sem_t1 = sem_open("/sem_t1", 0);
    sem_t2 = sem_open("/sem_t2", 0);
    if (sem_t1 < 0 || sem_t2 < 0) {
        printf("Error 002");
        exit(-1);
    }

    struct timeval timeval1;
    while (1) {
        int l_zadan = rand() % 100;
        int liczba = rand() % LIMIT;
        for (int j = 0; j < l_zadan; j++) {
            if (sem_wait(sem_t1) < 0) {
                printf("Error 003");
                exit(-1);
            }

            for (int i = 0; i < CZYTELNICY; i++) {
                if (sem_wait(sem_t2) < 0) {
                    printf("Error 004");
                    exit(-1);
                }
            }
            int index = rand() % SIZE;
            memory1->t[index] = liczba;
            gettimeofday(&timeval1, NULL);
            printf("(%d %ld.%ld) Wpisałem liczbę %d na pozycję %d. Pozostało %d zadań.\n", getpid(), timeval1.tv_sec,
                   timeval1.tv_usec / 1000, liczba, index, l_zadan - j - 1);
            liczba++;
            for (int i = 0; i < CZYTELNICY; i++) {
                if (sem_post(sem_t2) < 0) {
                    printf("Error 005");
                    exit(-1);
                }
            }

            if (sem_post(sem_t1) < 0) {
                printf("Error 006");
                exit(-1);
            }
        }
        sleep(1);
    }
}