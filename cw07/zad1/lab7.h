#ifndef LAB7_H
#define LAB7_H

#define SIZE 100
#define ID1 10
#define ID2 100

union semun {
    int val;                /* wartość dla SETVAL */
    struct semid_ds *buf;   /* bufor dla IPC_STAT i IPC_SET */
    ushort *array;          /* tablica dla GETALL i SETALL */
    struct seminfo *__buf;  /* bufor dla IPC_INFO */
    void *__pad;
};

struct bufor{
    int n;
    int get_pointer;
    int array[SIZE];
};

#endif