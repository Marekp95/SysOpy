#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include "zad2.h"

sem_t *sem_t1 = NULL;
sem_t *sem_t2 = NULL;
int shm1;
struct memory *memory1 = (struct memory *) -1;

void handler(int sig) {
    exit(0);
}

void clean() {
    if (sem_t1 != NULL) {
        sem_close(sem_t1);
        sem_unlink("/sem_t1");
    }
    if (sem_t2 != NULL ) {
        sem_close(sem_t2);
        sem_unlink("/sem_t2");
    }
    if (shm1 >= 0) {
        close(shm1);
        shm_unlink("/shm");
    }
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);
    srand(7);

    shm1 = shm_open("/shm", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if (shm1 < 0 || ftruncate(shm1, STRUCT_SIZE) < 0 ||
        (memory1 = (struct memory *) mmap(0, STRUCT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm1, 0)) < 0) {
        printf("Error 001\n");
        exit(-1);
    }
    for (int i = 0; i < SIZE; i++) {
        memory1->t[i] = rand() % LIMIT;
    }
    munmap(memory1, STRUCT_SIZE);
    sem_t1 = sem_open("/sem_t1", O_CREAT, S_IWUSR | S_IRUSR, 1);
    sem_t2 = sem_open("/sem_t2", O_CREAT, S_IWUSR | S_IRUSR, CZYTELNICY);
    if (sem_t1 < 0 || sem_t2 < 0) {
        printf("Error 002\n");
        exit(-1);
    }

    while (1)
        pause();
}