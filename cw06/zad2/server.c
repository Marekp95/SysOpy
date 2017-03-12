#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define LIMIT_OF_CLIENTS 5
#define NEW_CONNECTION 1
#define CONNECTED 2
#define GET_NEW_INT 3
#define SEND_RESULT 4
#define END_WORK 5
char msg[10000];
mqd_t clients[LIMIT_OF_CLIENTS];
mqd_t queue_id;
int flag;
void handler(int signal){
    flag = 0;
     msg[0]=END_WORK;
    for(int i=0;i<LIMIT_OF_CLIENTS;i++){
	if(clients[i]!=-1){
	 	mq_send(clients[i],msg,100,0);
		clients[i]=-1;
		mq_close(clients[i]);
	}
    }
    mq_close(queue_id);
}

int main(int argc, char * argv[]) {
    if(argc!=2){
        printf("Błędne argument\n");
        return -1;
    }

    struct mq_attr mq_attr1;
    mq_attr1.mq_flags=0;
    mq_attr1.mq_maxmsg=10;
    mq_attr1.mq_msgsize=100;

    queue_id = mq_open(argv[1],O_CREAT | O_RDONLY,S_IRUSR | S_IWUSR, &mq_attr1);
    if(queue_id==-1){
        printf("Error 001 %s\n",argv[1]);
        return -1;
    }

    
    for(int i=0;i<LIMIT_OF_CLIENTS;i++){
        clients[i]=-1;
    }
    signal(SIGINT,handler);
    srand(time(NULL));


    flag=1;
    int id,n;
    while(flag){
//printf("aaaaaaaaaaaa\n");
        if(mq_receive(queue_id,msg,10000,0)<0)
        {  printf("%d\n",errno);  continue;}
        //printf("%c\n",msg[0]);
        switch((int)msg[0]){
            case NEW_CONNECTION:
//printf("conn\n");
                for(int i =0;i<LIMIT_OF_CLIENTS;i++){
                    if(clients[i]==-1){
                        clients[i]=mq_open(msg+1,O_WRONLY,0,&mq_attr1);
                        msg[0]=CONNECTED;
                        sprintf(msg+1,"%d",i);
                        mq_send(clients[i],msg,100,0);
                        break;
                    }
                }
                break;
            case SEND_RESULT:
                if(msg[1]=='y') {
                    sscanf(msg+2,"%d,%d",&n,&id);
                    printf("Liczba pierwsza: %d (Klient: %d)\n", n, id);
                }
                break;
            case GET_NEW_INT:
                sscanf(msg+1,"%d",&id);
                msg[0]=GET_NEW_INT;
                sprintf(msg+1,"%d",rand()%100+1);
                mq_send(clients[id],msg,100,0);
                break;
            case END_WORK:
                sscanf(msg+1,"%d",&id);
                mq_close(clients[id]);
                clients[id]=-1;
                break;
            default:
                break;
        }
    }
    mq_close(queue_id);
    return 0;
}

