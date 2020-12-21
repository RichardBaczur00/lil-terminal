#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "command_utils.h"

#define BUFFER_SIZE 128
#define MAX_SPLITS 30

char *home, *restore;
int fdo, fdw;

int main() {
    char *cmd;

    int i;
    int exit_code = 0;
    int result;
    char *line = (char*) malloc(sizeof(char) * BUFFER_SIZE);
    char *buffer = (char*) malloc(sizeof(char) * BUFFER_SIZE);
    home = (char*) malloc(sizeof(char) * BUFFER_SIZE);
    home = getcwd(buffer, BUFFER_SIZE*sizeof(char));
    restore = (char*) malloc(sizeof(char) * BUFFER_SIZE);
    pid_t child;
    int stat;
    char *nl, *part;
    do {
        printf("/: &>");
        fgets(line, BUFFER_SIZE - 1, stdin);
        if (strcmp(line, "quit\n")) {
            nl = strdup(line);
            part = strtok(nl, ";\n");
            while (part != NULL) {
                char *c_restore = (char*) malloc(sizeof(char) * BUFFER_SIZE);
                int fd[2];
                cmd = strdup(part);
                if (pipe(fd) < 0) {
                    //error pipe
                    //TODO: Set status
                }
                if ((child=fork()) < 0) {
                    //error fork
                    //TODO: Set status
                }
                if (child == 0) {
                    result = exec_simple(cmd);
                    write(fd[1], restore, sizeof(char) * BUFFER_SIZE);
                    exit(1);
                } else {
                    close(fd[1]);
                }
                wait(&stat);
                read(fd[0], c_restore, sizeof(char) * BUFFER_SIZE);

                if (c_restore != NULL && strcmp(c_restore, "")) {
                    chdir(c_restore);
                }

                close(fd[0]);
                free(c_restore);
            }
            part = strtok(NULL, ";\n");
            free(nl);
            free(cmd);
        }
    } while (strcmp(line, "quit\n"));
    free(buffer);
    free(line);
    return 0;
}