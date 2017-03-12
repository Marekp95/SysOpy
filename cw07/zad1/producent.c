#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <zconf.h>
#include "lab7.h"

int semid1;
int semid2;
struct bufor * bufor1;


void clean() {
    shmdt(bufor1);
    return;
}


int main(int argc, char *argv[]) {
    atexit(clean);

    semid1 = shmget(ID1, (SIZE + 2) * sizeof(int), S_IWUSR);
    semid2 = semget(ID2, 2, S_IWUSR | S_IRUSR);
    if (semid1 <= 0 || semid2 < 0) {
        printf("error\n");
        return -1;
    }

    bufor1 = shmat(semid1, NULL, 0);
    if (bufor1 == (void *) -1) {
        printf("error\n");
        return -1;
    }
    struct sembuf sembuf1;
    sembuf1.sem_flg = 0;
    int t;
    int tn;
    int index;
    srand(7);
    struct timeval time;

    while (1) {
        sembuf1.sem_num = 1;
        sembuf1.sem_op = -1;
        semop(semid2, &sembuf1, 1);

        sembuf1.sem_num = 2;
        semop(semid2, &sembuf1, 1);

        tn = semctl(semid2, 0, GETVAL, NULL);
        index = (bufor1->get_pointer + bufor1->n) % SIZE;
        sembuf1.sem_op = 1;
        semop(semid2, &sembuf1, 1);

        int f = 0;
        t = rand()%100;
        if(bufor1->n<SIZE) {
            bufor1->array[index] = t;
            bufor1->n = bufor1->n + 1;
            f=1;
        }
        sembuf1.sem_num = 0;
        sembuf1.sem_op = 1;
        semop(semid2, &sembuf1, 1);

        gettimeofday(&time, NULL);
        if(f)
        printf("(%d %ld,%ld) Dodalem liczbe: %d na pozycji %d. Liczba zadan oczekujacych %d\n", getpid(), time.tv_sec,
               time.tv_usec / 1000, t, index, tn+1);
        sleep(1);
    }
}