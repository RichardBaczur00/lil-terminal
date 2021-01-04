#include "cd.h"

#define BUFFER_SIZE 128

extern char *home, *restore;

int cd(int argc, char *argv[]) {
    char *buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
    char *lastFolder = (char*)malloc(sizeof(char)*BUFFER_SIZE);
    char *actualFolder = (char*)malloc(sizeof(char)*BUFFER_SIZE);
    char *homeFolderSafe = strrchr(getcwd(buffer, BUFFER_SIZE*sizeof(char)), '/');

    bool verif = 0;

    if (strcmp(getcwd(buffer, BUFFER_SIZE*sizeof(char)), home) && strcmp(argv[1], "..")) verif = 1;

    lastFolder = strrchr(getcwd(buffer, BUFFER_SIZE*sizeof(char)), '/');
    if (argc <= 1) {
        printf("Error: too few arguments\n");
        return TOO_FEW_ARGUMENTS_ERROR;
    } else if (argc > 2) {
        printf("Error: too many arguments\n");
        return TOO_MANY_ARGUMENTS_ERROR;
    } else {
        if (chdir(argv[1])) {
            printf("Error: Cannot access directory\n");
            return CANNOT_ACCESS_ERROR;
        } else {
            actualFolder = strrchr(getcwd(buffer, BUFFER_SIZE*sizeof(char)), '/');
            if (strcmp(actualFolder, homeFolderSafe))
                if (verif)
                    printf("%s", lastFolder);
                else 
                    printf("%s", actualFolder);
        }    
    }
    strcpy(restore, argv[1]);

    return 0;
}