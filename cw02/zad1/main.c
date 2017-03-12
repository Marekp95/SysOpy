#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

int main(int argc,char * argv[]) {
    if(argc==4) {
        struct tms * tms1 = malloc(sizeof(struct tms));
        struct tms * tms2 = malloc(sizeof(struct tms));
        clock_t ct = clock();
        times(tms1);
        int record_size = atoi(argv[2]);
        record_size += 1;
        char *s = (char *)malloc((size_t) record_size - 1);
        char *s2 = (char *)malloc((size_t) record_size - 1);
        s[record_size - 1] = '\0';
        s2[record_size - 1] = '\0';
        if(strcmp(argv[3],"sys")==0) {
            printf("Use system\n");
            int f = open(argv[1], O_RDWR);


            long loc = lseek(f, 0, SEEK_END);

            int size = (int) ((loc + 2) / record_size);
            for (int i = 1; i < size; i++) {
                lseek(f, record_size * i, 0);
                read(f, s, (unsigned int) (record_size - 1));
                for (int j = i - 1; j >= 0; j--) {
                    lseek(f, record_size * j, 0);
                    read(f, s2, (unsigned int) (record_size - 1));
                    if (s2[0] > s[0]) {
                        lseek(f, record_size * (j + 1), 0);
                        write(f, s2, (unsigned int) (record_size - 1));
                        if (j == 0) {
                            lseek(f, record_size * (j), 0);
                            write(f, s, (unsigned int) (record_size - 1));
                        }
                    } else {
                        lseek(f, record_size * (j + 1), 0);
                        write(f, s, (unsigned int) (record_size - 1));
                        break;
                    }
                }
            }
            close(f);
        }else if(strcmp(argv[3],"lib")==0) {
            printf("Use library\n");
            FILE * file;
            file = fopen(argv[1],"r+");

            if(file!=NULL){
                fseek(file,0,SEEK_END);
                fpos_t size2;
                fgetpos(file,&size2);
                long size = size2.__pos;
                size+=2;
                size/=record_size;
                for(int i = 1;i<(int)size;i++){
                    fseek(file,record_size*i,0);
                    fread(s,1,(size_t)record_size-1,file);
                    for(int j = i-1;j>=0;j--){
                        fseek(file,j*record_size,0);
                        fread(s2,1,(size_t)record_size-1,file);
                        if(s2[0]>s[0]){
                            fseek(file,(j+1)*record_size,0);
                            fwrite(s2,1,(size_t)record_size-1,file);
                            if(j==0) {
                                fseek(file, (j) * record_size, 0);
                                fwrite(s, 1, (size_t) record_size - 1, file);
                            }
                        }else{
                            fseek(file,(j+1)*record_size,0);
                            fwrite(s,1,(size_t)record_size-1,file);
                            break;
                        }
                    }
                }
                fclose(file);
            }else{
                printf("File cannot be open\n");
            }
        }
        clock_t ct2 = clock();
        times(tms2);
        FILE * result = fopen("wyniki.txt","a");
        fprintf(result,"%s     Size: %d Real time: %ld System time: %ld User time: %ld\n",argv[3],record_size-1,ct2-ct,tms2->tms_stime-tms1->tms_stime,tms2->tms_utime-tms1->tms_utime);
    }else{
        printf("Bad parameters. Try: <location> <size> <lib | sys>");
    }
    return 0;
}