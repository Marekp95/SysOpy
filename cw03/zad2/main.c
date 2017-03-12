#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>

int fun(char * path,char * ext,int argc, char * argv[]){
    DIR * dir = opendir(path);
    if(dir==NULL)
        return -1;

    int file_counter = 0;
    int dir_counter = 0;
    struct dirent * dirent1;
    char file_name[FILENAME_MAX];
    strcpy(file_name,path);
    int length = (int)strlen(path);
    file_name[length]='/';
    length++;
    while((dirent1 = readdir(dir))!=NULL){
        if((strcmp(dirent1->d_name,"..")!=0)&&(strcmp(dirent1->d_name,".")!=0)){
            strncpy(file_name+length,dirent1->d_name,(size_t)((FILENAME_MAX)-length));
            struct stat stat1;
            stat(file_name,&stat1);
            if(S_ISREG(stat1.st_mode)){
                if(ext==NULL) {
		//printf("%s %s\n",file_name,dirent1->d_name);
                    file_counter++;
                }else{
                    char * e = strchr(dirent1->d_name,'.');
                    if(e!=NULL){
                        if(strcmp(e,ext)==0){
                            file_counter++;
			//printf("%s%s\n",file_name,dirent1->d_name);
                        }
                    }
                }
            }else if(S_ISDIR(stat1.st_mode)){
                dir_counter++;
                int res;
                int pid=fork();
                if(pid==0){
                    char * c = "PATH_TO_BROWSE=";
                    char * new_path = malloc((strlen(file_name)+strlen(c)+1)* sizeof(char));
                    strcpy(new_path,c);

                    strcpy(new_path+(strlen(c)* sizeof(char)),file_name);
                    new_path[strlen(file_name)+strlen(c)]='\0';
                    if(ext==NULL) {
                        char * env[] = {new_path, NULL};

                        res = execve(argv[0], argv, env);
                    }else{
                        char * c2 = "EXT_TO_BROWSE=";
                        char * new_ext = malloc((strlen(ext)+strlen(c2)+1)* sizeof(char));
                        strcpy(new_ext,c2);
                        strcpy(new_ext+(strlen(c2)* sizeof(char)),ext);
                        new_ext[strlen(file_name)+strlen(c2)]='\0';
                        char * env[] = {new_path,new_ext, NULL};
                        res = execve(argv[0], argv, env);
                        free(new_ext);
                    }
                    free(new_path);
                    _exit(res);
                }
            }

        }
    }
    for(int i = 1;i<argc;i++){
        if(strcmp(argv[i],"-w")==0){
            sleep(15);
            break;
        }
    }
    int other_file_counter = 0;
    for(int i = 0;i<dir_counter;i++){
        int status = -1;
        wait(&status);
        if(status!= -1)
            other_file_counter+=WEXITSTATUS(status);
        fflush(stdout);
    }
    for(int i = 1;i<argc;i++){
        if(strcmp(argv[i],"-v")==0){
            printf("%s own files: %d, sum: %d\n",path,file_counter,file_counter+other_file_counter);
            fflush(stdout);
            break;
        }
    }

    closedir(dir);
    exit(file_counter+other_file_counter);
}

int main(int argc, char *argv[]) {
    char * path = getenv("PATH_TO_BROWSE");
    char * ext = getenv("EXT_TO_BROWSE");

    if(path==NULL){
        path = "./";
}

    int res = fun(path,ext,argc,argv);
    printf("main res %d\n",res);
    return res;
}