#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <zconf.h>
#include <signal.h>

typedef struct samolot {
    int id;
    volatile int alive;
} samolot;


pthread_mutex_t mtx;
pthread_cond_t startuje;
pthread_cond_t laduje;
int liczba_samolotow;
int n;
int k;
volatile int czeka_na_start = 0;
volatile int czeka_na_ladowanie = 0;
volatile int wolny = 1;
volatile int na_pokladzie = 0;
pthread_t * thread_ids;
struct samolot *samoloty;

void clean() {
    for (int i = 0; i < liczba_samolotow; i++) {
        samoloty[i].alive = 0;
    }
    for (int i = 0; i < liczba_samolotow; i++) {
        pthread_join(thread_ids[i], NULL);
    }
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&startuje);
    pthread_cond_destroy(&laduje);
    free(thread_ids);
    free(samoloty);
}

void sighandler(int sig) {
    exit(0);
}

void czekaj_na_zezwolenie(struct samolot *samolot1, int tmp) {
    pthread_mutex_lock(&mtx);
    if (tmp == 1) { //start
        czeka_na_start++;
        printf("samolot %d czeka na zezwolenie startu\n",samolot1->id);
        while (!wolny) {
            pthread_cond_wait(&startuje, &mtx);
        }
        wolny = 0;
        czeka_na_start--;
    } else if (tmp == 0) { //ladowanie
        czeka_na_ladowanie++;
        printf("samolot %d czeka na zezwolenie do lądowania\n",samolot1->id);
        while (!wolny || na_pokladzie == n) {
            pthread_cond_wait(&laduje, &mtx);
        }
        wolny = 0;
        czeka_na_ladowanie--;
    }
    pthread_mutex_unlock(&mtx);
    return;
}

void free_air() {
    if (na_pokladzie < k) {
        if (czeka_na_ladowanie > 0) {
            pthread_cond_signal(&laduje);
        } else {
            pthread_cond_signal(&startuje);
        }
    } else {
        if (czeka_na_start > 0) {
            pthread_cond_signal(&startuje);
        } else if (na_pokladzie < n) {
            pthread_cond_signal(&laduje);
        }
    }
}

void ladowanie(struct samolot *p) {
    pthread_mutex_lock(&mtx);
    ++na_pokladzie;
    wolny = 1;
    free_air();
    printf("samolot %d wylądował, na pokładzie: %d, czekających na start: %d, czekających na lądowanie %d\n", p->id,na_pokladzie,czeka_na_start,czeka_na_ladowanie);
    pthread_mutex_unlock(&mtx);
}

void start(struct samolot *p) {
    pthread_mutex_lock(&mtx);
    na_pokladzie--;
    wolny = 1;
    free_air();
    printf("samolot %d wystartował, na pokładzie: %d, czekających na start: %d, czekających na lądowanie %d\n", p->id,na_pokladzie,czeka_na_start,czeka_na_ladowanie);
    pthread_mutex_unlock(&mtx);
}

void *fun(void *arg) {
    struct samolot *p = arg;
    while (p->alive) {
        czekaj_na_zezwolenie(p, 0);
        ladowanie(p);
        usleep(10000);
        czekaj_na_zezwolenie(p, 1);
        start(p);
        usleep(10000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Błędne argumenty\n");
        exit(1);
    }
    signal(SIGINT, sighandler);
    atexit(clean);
    srand(time(NULL));;
    liczba_samolotow = atoi(argv[1]);
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    thread_ids = malloc(liczba_samolotow * sizeof(pthread_t));
    samoloty = malloc(liczba_samolotow * sizeof(struct samolot));

    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&startuje, NULL);
    pthread_cond_init(&laduje, NULL);

    sigset_t mask;
    sigset_t sigset_t1;
    sigfillset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, &sigset_t1);

    for (int i = 0; i < liczba_samolotow; i++) {
        samoloty[i].id = i;
        samoloty[i].alive = 1;
        if (pthread_create(&thread_ids[i], NULL, fun, &samoloty[i]) != 0)
            exit(1);
    }
    pthread_sigmask(SIG_SETMASK, &sigset_t1, NULL);

    while (1) {
        pause();
    }
}