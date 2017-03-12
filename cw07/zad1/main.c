#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <bits/signum.h>
#include <signal.h>
#include <zconf.h>
#include "lab7.h"

int shm1,shm2;
void clean(){
    if(shm1>0)
        semctl(shm1, 0, IPC_RMID);
    if(shm2>0)
        semctl(shm2, 0, IPC_RMID);
    return;
}
void handler(int sig) {
    exit(0);
}

int main() {
    atexit(clean);
    signal(SIGINT, handler);
    shm1 = shmget(ID1, (SIZE + 2) * sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR);
    shm2 = semget(ID2, 3, IPC_CREAT | S_IWUSR | S_IRUSR);
    if (shm1 < 0 || shm2 < 0) {
        printf("error\n");
        exit(-1);
    }
    struct bufor * bufor1 = shmat(shm1, NULL, 0);
    bufor1->n=0;
    bufor1->get_pointer=0;
    union semun semun1;
    semun1.val = 0;
    semctl(shm2, 0, SETVAL, semun1);
    semun1.val = 1;
    semctl(shm2, 2, SETVAL, semun1);
    semun1.val = SIZE;
    semctl(shm2, 1, SETVAL, semun1);

    while (1)
        pause();
    return 0;
}