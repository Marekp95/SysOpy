#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
    if(argc<2) {
        printf("Błędne argumenty\n");
        return -1;
    }
    char * command1 = "ls -l | grep ^d > ";
    char * command2 = malloc(strlen(argv[1])+strlen(command1)+1);
    strcpy(command2,command1);
    strcpy(command2+strlen(command1),argv[1]);
    command2[strlen(argv[1])+strlen(command1)]='\0';
    FILE *cmd = popen(command2, "r");
    if (cmd == NULL) {
        printf("Error");
        exit(-1);
    }
    pclose(cmd);
    return 0;
}