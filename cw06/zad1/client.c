#include <stdlib.h>
#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <zconf.h>

#define NEW_CONNECTION 1
#define CONNECTED 2
#define GET_NEW_INT 3
#define SEND_RESULT 4
#define END_WORK 5

struct message{
    long msg_type;
    int id;
    int is_prime;
    long n;
};
int flag;
void handler(int signal){
    flag = 0;
}
int is_prima(long l){
    if(l==1)
        return 0;
    if(l==2||l==3||l==5)
        return 1;
    if(l%2==0||l%3==0||l%5==0)
        return 0;
    for(int i=7;i*i<=l;i+=2){
        if(l%i==0)
            return 0;
        i+=4;
        if(l%i==0)
            return 0;
    }
    return 1;
}
int main(int argc, char * argv[]) {
    if(argc!=3){
        printf("Błędne argument\n<path> <id>\n");
        return -1;
    }
    int n=atoi(argv[2]);
    if(n<=0){
        printf("Error 001\n");
        return -1;
    }

    key_t key_t1 = ftok(argv[1],n);
    if(key_t1==-1){
        printf("Error 002\n");
        return -1;
    }

    int server_id = msgget(key_t1,S_IRUSR | S_IWUSR);
    if(server_id==-1){
        printf("Error 003\n");
                return -1;
    }

    int queue_id = msgget(IPC_PRIVATE,S_IRUSR | S_IWUSR);
    if(queue_id==-1){
        printf("Error 004\n");
        return -1;
    }
    signal(SIGINT,handler);

    struct message msg;
    msg.msg_type = NEW_CONNECTION;
    msg.id=queue_id;
    if(msgsnd(server_id,&msg,sizeof(msg),0)<0){
        printf("Error 005\n");
        return -1;
    }
    if(msgrcv(queue_id,&msg,sizeof(struct message),CONNECTED,0)<0){
        printf("error 006\n");
        return -1;
    }
    int client_id=msg.id;
    flag=1;
    while(flag){
        msg.msg_type = GET_NEW_INT;
        msg.id=client_id;
        msgsnd(server_id,&msg,sizeof(msg),0);
        msgrcv(queue_id,&msg,sizeof(msg), GET_NEW_INT,0);
        printf("%ld\n",msg.n);
        msg.is_prime=is_prima(msg.n);
        msg.msg_type=SEND_RESULT;
        msgsnd(server_id,&msg,sizeof(msg),0);
        sleep(1);

    }
    msg.msg_type=END_WORK;
    msg.id=client_id;
    msgsnd(server_id,&msg,sizeof(msg),0);

    msgctl(queue_id,IPC_RMID,0);
    return 0;
}