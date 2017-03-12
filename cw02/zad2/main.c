#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <stdint.h>
#include <time.h>

#define DT_DIR 4
#define DT_FILE 8
int check_permission(mode_t mode, const char * arg){
    if(arg[0]=='r'){
        if(!(mode & S_IRUSR))
            return 0;
    }
    if(arg[0]=='-'){
        if((mode & S_IRUSR))
            return 0;
    }
    if(arg[1]=='w'){
        if(!(mode & S_IWUSR))
            return 0;
    }
    if(arg[1]=='-'){
        if((mode & S_IWUSR))
            return 0;
    }
    if(arg[2]=='x'){
        if(!(mode & S_IXUSR))
            return 0;
    }
    if(arg[2]=='-'){
        if((mode & S_IXUSR))
            return 0;
    }
    if(arg[3]=='r'){
        if(!(mode & S_IRGRP))
            return 0;
    }
    if(arg[3]=='-'){
        if((mode & S_IRGRP))
            return 0;
    }
    if(arg[4]=='w'){
        if(!(mode & S_IWGRP))
            return 0;
    }
    if(arg[4]=='-'){
        if((mode & S_IWGRP))
            return 0;
    }
    if(arg[5]=='x'){
        if(!(mode & S_IXGRP))
            return 0;
    }
    if(arg[5]=='-'){
        if((mode & S_IXGRP))
            return 0;
    }
    if(arg[6]=='r'){
        if(!(mode & S_IROTH))
            return 0;
    }
    if(arg[6]=='-'){
        if((mode & S_IROTH))
            return 0;
    }
    if(arg[7]=='w'){
        if(!(mode & S_IWOTH))
            return 0;
    }
    if(arg[7]=='-'){
        if((mode & S_IWOTH))
            return 0;
    }
    if(arg[8]=='x'){
        if(!(mode & S_IXOTH))
            return 0;
    }
    if(arg[8]=='-'){
        if((mode & S_IXOTH))
            return 0;
    }
    return 1;
}
void print_files(const char * path, const char * arg){
    DIR * dir = opendir(path);
    if(dir!=NULL) {
        struct dirent *dirent1 = readdir(dir);
        while (dirent1 != NULL) {

            if (strcmp(dirent1->d_name, ".") != 0 && strcmp(dirent1->d_name, "..") != 0) {
                char *path1 = malloc(strlen(path) + strlen(dirent1->d_name) + 2);
                strcpy(path1, path);
                strcat(strcat(path1, "/"), dirent1->d_name);
                struct stat stat1;
                stat(path1, &stat1);
                if (dirent1->d_type & DT_DIR) {
                    print_files(path1, arg);
                } else if (dirent1->d_type & DT_FILE) {
                    if (check_permission(stat1.st_mode, arg)) {
                        printf("%s\n", path1);
                        printf("File Permissions: \t");
                        printf((stat1.st_mode & S_IRUSR) ? "r" : "-");
                        printf((stat1.st_mode & S_IWUSR) ? "w" : "-");
                        printf((stat1.st_mode & S_IXUSR) ? "x" : "-");
                        printf((stat1.st_mode & S_IRGRP) ? "r" : "-");
                        printf((stat1.st_mode & S_IWGRP) ? "w" : "-");
                        printf((stat1.st_mode & S_IXGRP) ? "x" : "-");
                        printf((stat1.st_mode & S_IROTH) ? "r" : "-");
                        printf((stat1.st_mode & S_IWOTH) ? "w" : "-");
                        printf((stat1.st_mode & S_IXOTH) ? "x" : "-");

                        printf("\t%lu\t%s", stat1.st_size, ctime(&stat1.st_atime));
                    }
                }
            }
            dirent1 = readdir(dir);
        }
        closedir(dir);
    }else{
        printf("Error\n");
    }
}

int main(int argc,char * argv[]){
    if(argc==3) {
        if(strlen(argv[2])==9) {
            char *path = argv[1];
            print_files(path, argv[2]);
        }else{
            printf("Bad permission rights\n");
        }
    }else{
        printf("Wrong parameters\n");
    }
    return 0;
}