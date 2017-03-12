#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Błędne agrumenty\n");
        return -1;
    }
    int N = atoi(argv[1]);
    if (N < 0) {
        printf("Error\n");
        return -1;
    }

    int pipedes[2];
    if (pipe(pipedes) != 0) {
        printf("Error\n");
        exit(-1);
    }

    pid_t pid_t1 = fork();
    if (pid_t1 < 0) {
        printf("Error\n");
        exit(-1);
    }else if (pid_t1 == 0) {
        close(pipedes[0]);
        dup2(pipedes[1], 1);
        close(pipedes[1]);
        execlp("tr", "tr", "'[:lower:]'", "'[:upper:]'", NULL);

    }else {
        pid_t fold_pid = fork();
        if (fold_pid < 0) {
            kill(pid_t1, SIGINT);
            printf("Error\n");
            exit(-1);
        }else if (fold_pid == 0) {
            close(pipedes[1]);
            dup2(pipedes[0], 0);
            close(pipedes[0]);
            execlp("fold", "fold", "-w", argv[1], NULL);
        }else {
            close(pipedes[0]);
            close(pipedes[1]);
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}