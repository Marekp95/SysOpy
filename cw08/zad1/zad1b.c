#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <zconf.h>
#include <fcntl.h>
#include <sys/stat.h>

int fd;
size_t buffor_size;
pthread_t * pthread_t1;
pthread_mutex_t pthread_mutex_t1 = PTHREAD_MUTEX_INITIALIZER;
int start_flag = 0;
int threads_number;
int flag=1;

void * find(void *key) {
    char buffer[buffor_size][1025];
    if (pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL) != 0) {
        printf("Error 003\n");
        exit(-1);
    }
    while (start_flag == 0) usleep(1000);
    int i;
    do {
        i=0;
        while(pthread_mutex_trylock(&pthread_mutex_t1)){
            pthread_testcancel();
            usleep(1000);
        }

        long n = read(fd,buffer[i],1025*buffor_size);

        i=(int)(n/1025);
        for(int v=0;v<i;v++){
            buffer[v][1024]='\0';
        }

        pthread_mutex_unlock(&pthread_mutex_t1);
        for(int j=0;j<i;++j){
            char s[1025];
            int l;
            sscanf(buffer[j], "%d%s", &l, s);

            s[1024]='\0';

            if ((strstr(s, (char *) key)) != NULL) {

                printf("watek: %lu, rekord: %d \n", (unsigned long) pthread_self(),l);
                while(pthread_mutex_trylock(&pthread_mutex_t1)){
                    pthread_testcancel();
                    usleep(1000);
                }
                if(flag) {
                    for (int k = 0; k < threads_number; k++) {
                        if (pthread_t1[k] != pthread_self()) {
                            pthread_cancel(pthread_t1[k]);
                        }
                    }
                    flag=0;
                }

                pthread_mutex_unlock(&pthread_mutex_t1);
                return NULL;
            }
        }
        fflush(stdout);
        pthread_testcancel();
    }while(i==buffor_size);
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 5){
        printf("Błędne argumenty\n");
        return -1;
    }

    threads_number = atoi(argv[1]);
    if (threads_number == 0){
        printf("Ilość wątków musi być liczbą\n");
        return -1;
    }

    buffor_size = (size_t)atoi(argv[3]);
    if (buffor_size <= 0){
        printf("Podano błędny rozmiar bufora\n");
        return -1;
    }

    fd = open(argv[2],O_RDONLY,S_IREAD);
    if(fd==-1){
        printf("Błąd otwarcia pliku\n");
        return -1;
    }

    char * key = (char *) malloc((strlen(argv[4])+1) * sizeof(char));
    strcpy(key, argv[4]);
    key[strlen(argv[4])*sizeof(char)]='\0';

    pthread_t1 = (pthread_t *) malloc(threads_number * sizeof(pthread_t));

    for (int i = 0; i < threads_number; ++i) {
        if (pthread_create(&pthread_t1[i], NULL, find, key) != 0) {
            printf("Error 001\n");
            goto end;
        }
    }

    start_flag = 1;

    for (int i = 0; i < threads_number; ++i) {

        if (pthread_join(pthread_t1[i], NULL) != 0) {
            printf("Error 002\n");
            goto end;
        }
    }

    end:
    free(key);
    free(pthread_t1);
    close(fd);
    return 0;
}
