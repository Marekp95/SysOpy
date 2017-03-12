#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
void free_bufor(){
    char c;
    do{
        scanf("%c",&c);
    }while(c!='\n');
}
int main(int argc, char *argv[]) {
    if(argc==2) {
        int f = open(argv[1], O_RDWR);
        if (f >= 0) {
            printf("Commands: read-lock, write-lock, unlock, read, write, print, q\n");
            char *s = malloc(101);
            scanf("%100s", s);
            while (strcmp("q", s) != 0) {
                if (strcmp("read-lock", s) == 0) {
                    struct flock flock1;
                    flock1.l_type = F_RDLCK;
                    flock1.l_len = 1;
                    flock1.l_whence = SEEK_SET;
                    printf("Enter index of byte\n");
                    int offset;
                    scanf("%d", &offset);
                    flock1.l_start = offset;
                    flock1.l_pid = getpid();
                    if (fcntl(f, F_SETLK, &flock1) != 0) {
                        printf("error\n");
                    }
                } else if (strcmp("write-lock", s) == 0) {
                    struct flock flock1;
                    flock1.l_type = F_WRLCK;
                    flock1.l_len = 1;
                    flock1.l_whence = SEEK_SET;
                    printf("Enter index of byte\n");
                    int offset;
                    scanf("%d", &offset);
                    flock1.l_start = offset;
                    flock1.l_pid = getpid();
                    if (fcntl(f, F_SETLK, &flock1) != 0) {
                        printf("error\n");
                    }
                } else if (strcmp("read", s) == 0) {
                    printf("Enter index of byte\n");
                    int offset;
                    scanf("%d", &offset);
                    char c;
                    lseek(f, offset, SEEK_SET);
                    read(f, &c, 1);
                    printf("%c\n", c);
                } else if (strcmp("write", s) == 0) {
                    printf("Enter index of byte\n");
                    int offset;
                    scanf("%d", &offset);
                    printf("Insert character\n");
                    char c;
                    free_bufor();
                    scanf("%c", &c);
                    lseek(f, offset, SEEK_SET);
                    write(f, &c, 1);
                } else if (strcmp("unlock", s) == 0) {
                    struct flock flock1;
                    flock1.l_type = F_UNLCK;
                    flock1.l_len = 1;
                    flock1.l_whence = SEEK_SET;
                    printf("Enter index of byte\n");
                    int offset;
                    scanf("%d", &offset);
                    flock1.l_start = offset;
                    flock1.l_pid = getpid();
                    if (fcntl(f, F_SETLK, &flock1) != 0) {
                        printf("error\n");
                    }
                } else if (strcmp("print", s) == 0) {
                    long int file_size = lseek(f, 0, SEEK_END);
                    for (long int i = 0; i < file_size; i++) {
                        struct flock flock1;
                        flock1.l_type = F_WRLCK;
                        flock1.l_len = 1;
                        flock1.l_whence = SEEK_SET;
                        flock1.l_start = i;
                        if (fcntl(f, F_GETLK, &flock1) == 0) {
                            if (flock1.l_type == F_WRLCK) {
                                printf("Byte %ld, Write lock, PID: %d\n", i, flock1.l_pid);
                            } else if (flock1.l_type == F_RDLCK) {
                                printf("Byte %ld, Read lock, PID: %d\n", i, flock1.l_pid);
                            }
                        } else {
                            printf("error\n");
                        }
                    }
                } else {
                    printf("Command not found\n");
                }
                scanf("%100s", s);
            }
            close(f);
        }else{
            printf("File not found\n");
        }
    }else{
        printf("Bad argument\n");
    }
    return 0;
}