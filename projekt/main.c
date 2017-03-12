#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include "main.h"
#include <time.h>
#include <bits/sigset.h>

#define LICZBA_TOROW_DOJAZDOWYCH 2
#define LEWY 0
#define PRAWY 1

sem_t *sem_t1 = NULL;
sem_t *tunel = NULL;
sem_t *sem_state = NULL;

sem_t *dojazd[2][2];

int shm[4];
struct memory *tor[2][2];
int pid_zarzadca = 0;

void handler(int sig) {
    if (sig != SIGUSR1)
        exit(0);
}

void clean() {
    if (sem_t1 != NULL) {
        sem_close(sem_t1);
        sem_unlink("/sem_t1");
    }
    if (tunel != NULL) {
        sem_close(tunel);
        sem_unlink("/tunel");
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (dojazd[i][j] != NULL) {
                sem_close(dojazd[i][j]);
                char c[20];
                sprintf(c, "dojazd%d%d", i, j);
                sem_unlink(c);
            }
        }
    }
    if (sem_state != NULL) {
        sem_close(sem_state);
        sem_unlink("/sem_state");
    }
    if (shm[0] >= 0) {
        close(shm[0]);
        shm_unlink("/shm1");
    }
    if (shm[1] >= 0) {
        close(shm[1]);
        shm_unlink("/shm2");
    }
    if (shm[2] >= 0) {
        close(shm[2]);
        shm_unlink("/shm3");
    }
    if (shm[3] >= 0) {
        close(shm[3]);
        shm_unlink("/shm4");
    }
    for (int i = 0; i < LICZBA_TOROW_DOJAZDOWYCH; i++) {
        munmap(tor[LEWY][i], STRUCT_SIZE);
        munmap(tor[PRAWY][i], STRUCT_SIZE);
    }
}

void print_stan(int p, int n) {//funkcja rysująca aktualny stan torów dojazdowych i pociągu w tunelu
    sem_wait(sem_t1);
    int tl[LICZBA_TOROW_DOJAZDOWYCH];
    for (int i = 0; i < LICZBA_TOROW_DOJAZDOWYCH; i++) {
        tl[i] = 0;
    }
    for (int tmp = 0; tmp < LICZBA_TOROW_DOJAZDOWYCH; tmp++) {
        for (int i = tor[LEWY][tmp]->add_id; i != tor[LEWY][tmp]->get_id; i = (i - 1 + SIZE) % SIZE) {
            tl[tmp] *= 10;
            tl[tmp] += tor[LEWY][tmp]->typ[(i - 1 + SIZE) % SIZE];
        }
    }
    int tp[LICZBA_TOROW_DOJAZDOWYCH];
    for (int i = 0; i < LICZBA_TOROW_DOJAZDOWYCH; i++) {
        tp[i] = 0;
    }
    for (int tmp = 0; tmp < LICZBA_TOROW_DOJAZDOWYCH; tmp++) {
        for (int i = tor[PRAWY][tmp]->get_id; i != tor[PRAWY][tmp]->add_id; i = (i + 1) % SIZE) {
            tp[tmp] *= 10;
            tp[tmp] += tor[PRAWY][tmp]->typ[i];
        }
    }
    printf("\n");
    printf("%10d                          %d\n", tl[0], tp[0]);
    printf("            ");
    for (int i = 0; i < n; i++) {
        printf("=");
    }
    printf("%d", p);
    for (int i = n; i < 19; i++) {
        printf("=");
    }
    printf("\n");
    printf("%10d                          %d\n", tl[1], tp[1]);
    printf("\n");
    fflush(stdout);
    sem_post(sem_t1);
}

void start_pociag(int tt, int i, int typ) {
    srand(time(NULL));

    tor[tt][i]->t[tor[tt][i]->add_id] = getpid();//id pociagu
    typ = 1 + rand() % 3;
    tor[tt][i]->typ[tor[tt][i]->add_id] = typ;//typ pociagu
    tor[tt][i]->is_signalized[tor[tt][i]->add_id] = FALSE;
    tor[tt][i]->add_id = (tor[tt][i]->add_id + 1) % SIZE;
    tor[tt][i]->sum_of_priority += typ;

    //pociag jest dodant do kolejki
    //czekamy na syglal, ze mozemy wjezdzac do tunelu
    sigset_t sigset_t1;
    sigprocmask(SIGUSR1, &sigset_t1, NULL);
    sigsuspend(&sigset_t1);//otrzymuje sygnał jeśli jest na pierwszej pozycji w torze dojazdowym

    //czekamy na możliwość wjazdu do tunelu
    sem_wait(dojazd[tt][i]);

    //wjezdzamy do tunelu
    sem_wait(tunel);
    printf("pociag: %d   wjechal do tunelu od strony: %d z toru: %d priorytet: %d\n", getpid(), tt, i, typ);

    if (tt == 0) {//jedziemy pociągiem od lewej do prawej
        for (int i = 0; i < 20; i++) {
            for (int i = 0; i < 20; i++)
                printf("\n");
            print_stan(typ, i);
            usleep(50000);
        }
    } else {//jedziemy pociagiem od prawej do lewej
        for (int i = 19; i >= 0; i--) {
            for (int i = 0; i < 20; i++)
                printf("\n");
            print_stan(typ, i);
            usleep(50000);
        }
    }
    printf("pociag: %d   wyjechal z tunelu  \n", getpid());
    sem_post(tunel);
    //semaforem sygnalizujemy zarządcy, że pociąg wyjechał z tunelu
    sem_post(sem_state);
    //wyjechalismy z tunelu
    //konczymy proces tego pociagu
}

void zarzadca() {

    while (1) {
        //obslugujemy kolejny pociag
        sem_wait(sem_t1);
        int rekl = -1;
        int torl = -1;
        for (int i = 0; i < LICZBA_TOROW_DOJAZDOWYCH; i++) {
            int tmp = tor[LEWY][i]->sum_of_priority;
            if (tmp > 0) {
                tmp += tor[LEWY][i]->bonus;
                tor[LEWY][i]->bonus += 1;
            }
            if (tmp > rekl) {
                torl = i;
                rekl = tmp;
            }
        }
        int rekp = -1;
        int torp = -1;
        for (int i = 0; i < LICZBA_TOROW_DOJAZDOWYCH; i++) {
            int tmp = tor[PRAWY][i]->sum_of_priority;
            if (tmp > 0) {
                tmp += tor[PRAWY][i]->bonus;
                tor[PRAWY][i]->bonus += 1;
            }
            if (tmp > rekp) {
                torp = i;
                rekp = tmp;
            }
        }
        for (int i = 0;
             i < 2; i++) {//wysyłąnie sygnałów do pociągów, które są na początku tuneli, a jeszcze nie otrzymały sygnału
            for (int j = 0; j < LICZBA_TOROW_DOJAZDOWYCH; j++) {
                if (tor[i][j]->add_id != tor[i][j]->get_id) {//jeśli tor nie jest pusty
                    if (tor[i][j]->is_signalized[tor[i][j]->get_id] ==
                        FALSE) {//jeśli sygnał do danego pociągu nie został jeszcze wysyłany
                        //wysyłamy sygnał
                        int pid = tor[i][j]->t[tor[i][j]->get_id];
                        kill(pid, SIGUSR1);
                        //zmieniamy status wysłania sygnału
                        tor[i][j]->is_signalized[tor[i][j]->get_id] = TRUE;
                    }
                }
            }
        }
        sem_post(sem_t1);

        //identyfikujemy pociąg, który wpuszczamy do tunelu
        int strona = -1;
        if (rekp != 0 || rekl != 0)
            strona = rekp > rekl ? PRAWY : LEWY;
        int nr_tor = rekp > rekl ? torp : torl;

        if (strona != -1) {
            //zajmujemy semafor, który będzie zwolniony kiedy pociąg wyjedzie z tunelu
            sem_wait(sem_state);
            //zwalniamy semafor aby zasygnalizować pociągowi możliwość wjazdu do tunelu
            sem_post(dojazd[strona][nr_tor]);

            //usuwamy pociag z kolejki do tunelu
            sem_wait(sem_t1);
            tor[strona][nr_tor]->sum_of_priority -= tor[strona][nr_tor]->typ[tor[strona][nr_tor]->get_id];
            tor[strona][nr_tor]->bonus = 0;//zerujemy bonus po puszczeniu jednego pociagu

            tor[strona][nr_tor]->get_id = (tor[strona][nr_tor]->get_id + 1) % SIZE;
            sem_post(sem_t1);

            //czekamy aż pociąg wyjedzie z tunelu (zwalnia wtedy semafor)
            sem_wait(sem_state);
            sem_post(sem_state);
        } else {//nie było żadnych czekających pociągów
            usleep(10000);
        }
    }
}

int main(int argc, char *argv[]) {
    atexit(clean);
    signal(SIGINT, handler);
    signal(SIGUSR1, handler);
    srand(time(NULL));

    shm[0] = shm_open("/shm1", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    shm[1] = shm_open("/shm2", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    shm[2] = shm_open("/shm3", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    shm[3] = shm_open("/shm4", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    for (int tmp = 0; tmp < LICZBA_TOROW_DOJAZDOWYCH; tmp++) {
        if (shm[tmp] < 0 || ftruncate(shm[tmp], STRUCT_SIZE) < 0 ||
            (tor[LEWY][tmp] = (struct memory *) mmap(0, STRUCT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm[tmp], 0)) <
            0) {
            printf("Error 001\n");
            exit(-1);
        }

        tor[LEWY][tmp]->add_id = 0;
        tor[LEWY][tmp]->get_id = 0;
        tor[LEWY][tmp]->sum_of_priority = 0;
        tor[LEWY][tmp]->bonus = 0;
    }
    for (int tmp = 0; tmp < LICZBA_TOROW_DOJAZDOWYCH; tmp++) {
        if (shm[tmp + LICZBA_TOROW_DOJAZDOWYCH] < 0 ||
            ftruncate(shm[tmp + LICZBA_TOROW_DOJAZDOWYCH], STRUCT_SIZE) < 0 ||
            (tor[PRAWY][tmp] = (struct memory *) mmap(0, STRUCT_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                                                      shm[tmp + LICZBA_TOROW_DOJAZDOWYCH], 0)) < 0) {
            printf("Error 001\n");
            exit(-1);
        }

        tor[PRAWY][tmp]->add_id = 0;
        tor[PRAWY][tmp]->get_id = 0;
        tor[PRAWY][tmp]->sum_of_priority = 0;
        tor[PRAWY][tmp]->bonus = 0;
    }

    sem_t1 = sem_open("/sem_t1", O_CREAT, S_IWUSR | S_IRUSR, 1);
    tunel = sem_open("/tunel", O_CREAT, S_IWUSR | S_IRUSR, 1);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            dojazd[i][j] = NULL;
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            char c[20];
            sprintf(c, "dojazd%d%d", i, j);
            dojazd[i][j] = sem_open(c, O_CREAT, S_IWUSR | S_IRUSR, 1);
            if (dojazd[i][j] == NULL)
                exit(-1);
            //zajmujemy semafor
            sem_wait(dojazd[i][j]);
        }
    }
    sem_state = sem_open("/sem_state", O_CREAT, S_IWUSR | S_IRUSR, 1);
    if (sem_t1 < 0 || tunel < 0 || sem_state < 0) {
        printf("Error 002\n");
        exit(-1);
    }

    pid_zarzadca = fork();
    if (pid_zarzadca == 0) {
        zarzadca();
    }

    while (1) {
        int tt = rand() % 2;
        int i = rand() % LICZBA_TOROW_DOJAZDOWYCH;
        int typ = rand() % 3 + 1;
        sem_wait(sem_t1);
        int tmp = 0;
        if ((tor[tt][i]->add_id + 1) % SIZE != tor[tt][i]->get_id)
            tmp = 1;
        sem_post(sem_t1);
        if (tmp == 1) {//wtedy dodajemy pociag, bo wiemy, ze jest miejsce w kolejce
            int pid = fork();
            if (pid == 0) {
                start_pociag(tt, i, typ);
                exit(0);
            }
        }

        usleep(rand() % 1000000);
    }
}
