#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


#include "ls.h"

bool setL = 0, setA = 0, setF = 0, setS = 0, noFlag = 1;

int ls(int argc, char *argv[]) {
    printf("printing ls");
    int noFiles = 0;
    char *files[NUMBER_OF_FILES];
    int result = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            setL = 1;
            noFlag = 0;
        } else if (strcmp(argv[i], "-a") == 0) {
            setA = 1;
            noFlag = 0;
        } else if (strcmp(argv[i], "-F") == 0) {
            setF = 1;
            noFlag = 0;
        } else if (strcmp(argv[i], "-s") == 0) {
            setS = 1;
            noFlag = 0;
        } else {
            files[noFiles] = (char*)malloc(sizeof(char*)*strlen(argv[i]));
            strcpy(files[noFiles], argv[i]);
            noFiles++;
        }
    }

    if (noFiles == 0) {
        result = ls_d(".");
    } else {
        for (int i = 0; i < noFiles; ++i) {
            result = ls_d(files[i]);
        }
    }
}

int ls_d(char *source) {
    int result = 0;
    struct dirent *de;
    struct stat *buf;
    struct stat sa;
    struct group *group;
    struct passwd *passwd;
    char *time;
    DIR *d;
    char symb[2];
    de = (struct dirent*)malloc(sizeof(struct dirent));
    if ((d=opendir(source)) < 0) {
        printf("Directory %s cannot be accessed", source);
        return NOT_FOUND;
    }

    while ((de = readdir(d)) != NULL) {
        stat(de->d_name, &sa);

        if (setL) {
            if (setS) {
                printf("%3d ", (int)sa.st_blocks);
            }

            if(S_ISDIR(sa.st_mode)) printf("d");
			  else printf("-");
			 if(sa.st_mode & S_IRUSR) printf("r");
			  else printf("-");
			 if(sa.st_mode & S_IWUSR) printf("w");
			  else printf("-");
			 if(sa.st_mode & S_IXUSR) printf("x");
			  else printf("-");
			 if(sa.st_mode & S_IRGRP) printf("r");
			  else printf("-");
			 if(sa.st_mode & S_IWGRP) printf("w");
			  else printf("-");
		     if(sa.st_mode & S_IXGRP) printf("x");
			  else printf("-");
			 if(sa.st_mode & S_IROTH) printf("r");
			  else printf("-");
			 if(sa.st_mode & S_IWOTH) printf("w");
			  else printf("-");
			 if(sa.st_mode & S_IXOTH) printf("x");
			  else printf("-");

            if (de->d_name != 0) {
                if (setF) {
                    if (S_ISDIR(sa.st_mode)) strcpy(symb, "/");
                    else {
                        if (S_ISREG(sa.st_mode)) strcpy(symb, " ");
                        if (is_exec(de->d_name)) strcpy(symb, "*");
                        if (S_ISLNK(sa.st_mode)) strcpy(symb, "@");
                    }
                } else strcpy(symb, " ");
                stat(de->d_name, &sa);
                time = (char*)ctime(&sa.st_atime);
                time[16] = '\0';
                time += 4;
                passwd = getpwuid(sa.st_uid);
                group = getgrgid(sa.st_gid);
                if (setA) {
                    printf("%3u %-8s %-7s %9d %s %s%s\n", sa.st_nlink, passwd->pw_name, group->gr_name, sa.st_size, time, de->d_name, symb);
                } else {
                    if(strcmp(de->d_name,".")!=0&&strcmp(de->d_name,"..")!=0)
				        if(de->d_name[0]!='0')
					        printf("%3u %-8s %-7s %9d %s %s%s\n",sa.st_nlink, passwd->pw_name, group->gr_name,sa.st_size,time, de->d_name,symb);
                }
            }
        } else {
            if (setS)
            {
                if (setA)
                    printf("%3d", sa.st_blocks);
                else
                    if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
                        if (de->d_name[0] != '0')
                            printf("%3d", sa.st_blocks);
            }

            if (setF) {
                if (S_ISDIR(sa.st_mode)) {
                    strcpy(symb, "/");
                }
                else {
                    if (S_ISDIR(sa.st_mode)) strcpy(symb, "/");
                    else {
                        if (S_ISREG(sa.st_mode)) strcpy(symb, " ");
                        if (is_exec(de->d_name)) strcpy(symb, "*");
                        if (S_ISLNK(sa.st_mode)) strcpy(symb, "@");
                    }
                }
            } else strcpy(symb, " ");
            int i = 0;
            if (setA) {
                printf("%s%s\n", de->d_name, symb);
                ++i;
            } else {
                if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                    printf("%s%s\n", de->d_name, symb);
                }
            }
        }
    }

    printf("\n");
}


int is_exec(char *name) {
    if (strrchr(name, '.')) {
        return 0;
    }
    return 1;
}