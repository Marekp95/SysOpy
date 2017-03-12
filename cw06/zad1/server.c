#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#define LIMIT_OF_CLIENTS 5
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

    int server_id = msgget(key_t1,IPC_CREAT | S_IRUSR | S_IWUSR);
    if(server_id==-1){
        printf("Error 003\n");
        return -1;
    }

    int clients[LIMIT_OF_CLIENTS];
    for(int i=0;i<LIMIT_OF_CLIENTS;i++){
        clients[i]=-1;
    }
    signal(SIGINT,handler);
    srand(time(NULL));
    flag=1;
    struct message msg;
    while(flag){
        if(msgrcv(server_id,&msg,sizeof(msg),0,IPC_NOWAIT)<0)
            continue;
        switch(msg.msg_type){
            case NEW_CONNECTION:
                for(int i =0;i<LIMIT_OF_CLIENTS;i++){
                    if(clients[i]==-1){
                        clients[i]=msg.id;
                        msg.msg_type=CONNECTED;
                        msg.id=i;
                        msgsnd(clients[i],&msg,sizeof(msg),0);
                        break;
                    }
                }
                break;
            case SEND_RESULT:
                if(msg.is_prime)
                    printf("Liczba pierwsza: %ld (Klient: %d)\n",msg.n,msg.id);
                break;
            case GET_NEW_INT:
                msg.n=rand()%100+1;
                msgsnd(clients[msg.id],&msg,sizeof(msg),0);
                break;
            case END_WORK:
                clients[msg.id]=-1;
                break;
            default:
                break;
        }
    }
    msgctl(server_id,IPC_RMID,NULL);
    printf("Dgdfgdfg");
    return 0;
}
