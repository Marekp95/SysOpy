#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>


int vfork(void);
int counter = 0;

int fun(void * w){
clock_t t=clock();
    counter++;
_exit((int)(clock()-t));
}
int main(int argc, char * argv[]) {
    if(argc<3){
        printf("Bad arguments. Try: <type 1-4> <N>\n");
        return 0;
    }
    FILE * f = fopen("res.txt","a");
    if(f==NULL){
         printf("File cannot be open\n");
         return(0);
    }
    clock_t clock_t1 = clock();
    struct tms tms4;
    int stack_size = 1410;
    void * stack = mmap(0,(size_t)stack_size,PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE,0,0);
    stack+=stack_size;
    long int child_time=0;
    int N = atoi(argv[2]);
    //fork
    if(strcmp(argv[1],"1")==0) {
        for (int i = 0; i < N; i++) {
            int pid = fork();
            if (pid == 0) {
                fun(NULL);
            } else {
                int stat;
                wait(&stat);
                child_time+=WEXITSTATUS(stat);
            }
        }
    }else if(strcmp(argv[1],"2")==0) {
        //fork
        for (int i = 0; i < N; i++) {
            int pid = vfork();
            if (pid == 0) {
                fun(NULL);
            } else {
                int stat;
                wait(&stat);
                child_time+=WEXITSTATUS(stat);
            }
        }
    }else if(strcmp(argv[1],"3")==0) {
        //clone fork
        for (int i = 0; i < N; i++) {
            clone(fun, stack, SIGCHLD, NULL);
            int stat;
            wait(&stat);
            child_time+=WEXITSTATUS(stat);
        }
    }else if(strcmp(argv[1],"4")==0) {
        //clone vfork
        for(int i = 0; i<N;i++) {
            clone(fun, stack, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
            int stat;
            wait(&stat);
            child_time+=WEXITSTATUS(stat);
        }
    }else{
	printf("Bad type\n");
        goto end;
    }

    times(&tms4);
    clock_t clock_t2 = clock();
    /*printf("w1 User time%ld\n",tms4.tms_utime);
    p:intf("w1 System time%ld\n",tms4.tms_stime);
    printf("w1 S+U time%ld\n",tms4.tms_stime+tms4.tms_utime);
    printf("w1 Real time: %ld\n",((clock_t2-clock_t1)*100)/CLOCKS_PER_SEC);
    printf("w2 User time%ld\n",tms4.tms_cutime);
    printf("w2 System time%ld\n",tms4.tms_cstime);
    printf("w2 S+U time%ld\n",tms4.tms_cstime+tms4.tms_cutime);
    printf("w2  Real time: %ld\n",(child_time*100)/CLOCKS_PER_SEC);
    printf("w3 User time%ld\n",tms4.tms_utime+tms4.tms_cutime);
    printf("w3 System time%ld\n",tms4.tms_stime+tms4.tms_cstime);
    printf("w3 S+U time%ld\n",tms4.tms_stime+tms4.tms_cstime+tms4.tms_cutime+tms4.tms_utime);
    printf("w3 Real time: %ld\n",((clock_t2-clock_t1+child_time)*100)/CLOCKS_PER_SEC);*/
    printf("%d\n",counter);
 
    long t11=tms4.tms_utime;
    long t12=tms4.tms_stime;
    long t13=tms4.tms_stime+tms4.tms_utime;
    long t14=((clock_t2-clock_t1)*100)/CLOCKS_PER_SEC;
    long t21=tms4.tms_cutime;
    long t22=tms4.tms_cstime;
    long t23=tms4.tms_cstime+tms4.tms_cutime;
    long t24=(child_time*100)/CLOCKS_PER_SEC;
    long t31=tms4.tms_utime+tms4.tms_cutime;
    long t32=tms4.tms_stime+tms4.tms_cstime;
    long t33=tms4.tms_stime+tms4.tms_cstime+tms4.tms_cutime+tms4.tms_utime;
    long t34=((clock_t2-clock_t1+child_time)*100)/CLOCKS_PER_SEC;
    fprintf(f,"%d,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",N,argv[1],t11,t12,t13,t14,t21,t22,t23,t24,t31,t32,t33,t34);
    end:
    munmap(stack-stack_size,(size_t)stack_size);
    fclose(f);
    return 0;
}
