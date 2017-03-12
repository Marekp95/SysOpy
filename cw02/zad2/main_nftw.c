#include <stdio.h>
#include <ftw.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>

typedef struct FTW FTW;
char * realpath(const char * , const char * );
void nftw(char *path, int (*info)(const char *, const struct stat *, int, struct FTW *), int i, int flag);
char * permissions;
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
int fn (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if(typeflag==FTW_F) {
        if(check_permission(sb->st_mode,permissions)) {
	char p[PATH_MAX];
        	realpath(fpath, p);
            printf("%s\n", p);
            printf("File Permissions: \t");
            printf((sb->st_mode & S_IRUSR) ? "r" : "-");
            printf((sb->st_mode & S_IWUSR) ? "w" : "-");
            printf((sb->st_mode & S_IXUSR) ? "x" : "-");
            printf((sb->st_mode & S_IRGRP) ? "r" : "-");
            printf((sb->st_mode & S_IWGRP) ? "w" : "-");
            printf((sb->st_mode & S_IXGRP) ? "x" : "-");
            printf((sb->st_mode & S_IROTH) ? "r" : "-");
            printf((sb->st_mode & S_IWOTH) ? "w" : "-");
            printf((sb->st_mode & S_IXOTH) ? "x" : "-");
            __time_t tt = sb->st_atime;
            time_t t = tt;
            printf("\t%lu\t%s\n", sb->st_size, ctime(&t));
        }
    }
    return 0;
}
int main(int argc,char * argv[]) {
    if(argc==3) {
        if(strlen(argv[2])==9) {

            permissions = argv[2];

            nftw(argv[1], fn, 10, 0);
        }
    }else{
	printf("Bad arguments\n");
}
    return 0;
}