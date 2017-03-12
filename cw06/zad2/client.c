#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

#define NEW_CONNECTION 1
#define CONNECTED 2
#define GET_NEW_INT 3
#define SEND_RESULT 4
#define END_WORK 5

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
    if(argc!=2){
        printf("Błędne argument\n");
        return -1;
    }

    char name[100];
    sprintf(name,"/client%d",getpid());

    struct mq_attr mq_attr1;
    mq_attr1.mq_flags=0;
    mq_attr1.mq_maxmsg=10;
    mq_attr1.mq_msgsize=100;
//printf("%s\n",name);

mqd_t server_id = mq_open(argv[1], O_WRONLY, 0, &mq_attr1);
    if(server_id==-1){
        printf("Error 002\n");
        //mq_close(queue_id);
        return -1;
    }

    mqd_t queue_id = mq_open(name,O_CREAT | O_EXCL|O_RDONLY, S_IRUSR | S_IWUSR,&mq_attr1);
    if(queue_id==-1){
        printf("Error 001 %d\n",errno);
        return -1;
    }

    

   

    char msg[110];
    msg[0]=NEW_CONNECTION;
    strcpy(msg+1,name);
//printf("%s\n",msg+1);
    if(mq_send(server_id,msg,100,0)<0){
        printf("Error 005\n");
        return -1;
    }
//printf("aaaa\n");
    while(mq_receive(queue_id,msg,100,0)==-1)printf("v\n");
    signal(SIGINT,handler);
    int client_id;
    sscanf(msg+1,"%d",&client_id);
//printf("qqqqqqqqqqqq\n");
    flag=1;
    while(flag){
//printf("petla\n");
        msg[0] = GET_NEW_INT;
        sprintf(msg+1,"%d",client_id);
        mq_send(server_id,msg,100,0);
        while(mq_receive(queue_id,msg,100,NULL)==-1);
	if(msg[0]==END_WORK)
	    goto end;
        int n;
        sscanf(msg+1,"%d",&n);
        printf("%d\n",n);
        if(is_prima(n))
            msg[1]='y';
        else
            msg[1]='n';
        msg[0]=SEND_RESULT;
        sprintf(msg+2,"%d,%d",n,client_id);
        mq_send(server_id,msg,100,0);
        sleep(1);

    }
    msg[0]=END_WORK;
    sprintf(msg+1,"%d",client_id);
    mq_send(server_id,msg,100,0);
end:
    mq_close(server_id);    
    mq_close(queue_id);
    return 0;
}
