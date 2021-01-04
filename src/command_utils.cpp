#include "command_utils.h"

#define ARGUMENT_NUMBER 16
#define PIPE_OPEN_ERROR 0x00001
#define PROCESS_CREATION_ERROR 0x00010
#define PIPE_READ_ERROR 0x00100
#define FILE_DESCRIPTOR_DUP_ERROR 0x01000
#define FILE_OPEN_ERROR 0x10000
#define SIZEOF_INT sizeof(int)

int exec_simple(char* line) {
    int i;
    int result;
    char *str;
    char sep[3];
    char *cmd_one;
    char *cmd_two;
    int status;
    int fd[2];
    char buff[SIZEOF_INT];
    pid_t child_two;

    if (str = strstr(line, "&&")) {
        cmd_one = strdup(line);
        cmd_one[strlen(cmd_one) - strlen(str)] = 0;
        cmd_two = strdup(str + 2);
        strcpy(sep, "&&");
    } else if (str = strstr(line, "||")) {
        cmd_one = strdup(line);
        cmd_one[strlen(cmd_one) - strlen(str)] = 0;
        cmd_two = strdup(str + 2);
        strcpy(sep, "||");
    } else {
        result = double_redirect(line);
        return result;
    }

    if (pipe(fd) < 0) {
        //Error open pipe
        free(cmd_one);
        free(cmd_two);
        return PIPE_OPEN_ERROR;
    } 
    
    if ((child_two = fork()) < 0) {
        free(cmd_one);
        free(cmd_two);
        return PROCESS_CREATION_ERROR;
    }

    if (child_two == 0) {
        close(fd[0]);
        result = double_redirect(cmd_one);
        sprintf(buff, "%d", result);
        write(fd[1], buff, SIZEOF_INT);
        close(fd[1]);
        exit(0);
    } else {
        close(fd[1]);
        if (read(fd[0], buff, SIZEOF_INT) < 0) {
            free(cmd_one);
            free(cmd_two);
            return PIPE_READ_ERROR;
        }
        
        if (strcmp(sep, "&&") == 0) {
            if (strcmp(buff, "0") == 0) {
                result = exec_simple(cmd_two);
            }
        }

        if (strcmp(sep, "||") == 0) {
            if (strcmp(buff, "-1") == 0) {
                result = exec_simple(cmd_two);
            }
        }

        wait(&status);
        close(fd[0]);
    }
    free(cmd_one);
    free(cmd_two);

    return result;
}

int double_redirect(char *line) {
    pid_t child_two;
    int status;
    int result;
    char *str;
    int n;
    int fd[2], new_fdw;
    char *cmd_one, *cmd_two;
    int r = 1;

    if (str == strchr(line, '|')) {
        if (str[1] != '|') {
            r = 0;
            cmd_one = strdup(line);
            cmd_one[strlen(cmd_one) - strlen(str)] = 0;
            cmd_two = strdup(str + 1);
        }
    }

    if (r) {
        result = redirect(line);

        return result;
    }

    if (pipe(fd) < 0) {
        free(cmd_one);
        free(cmd_two);
        return PIPE_OPEN_ERROR;
    }

    if ((child_two = fork()) < 0) {
        free(cmd_one);
        free(cmd_two);
        return PROCESS_CREATION_ERROR;
    }

    if (child_two == 0) {
        close(fd[0]);

        if ((new_fdw = dup2(fd[1], 1)) < 0) {
            free(cmd_one);
            free(cmd_two);
            return FILE_DESCRIPTOR_DUP_ERROR;
        }

        result = redirect(cmd_one);
        close(fd[1]);
        exit(1);
    } else {
        close(fd[1]);
        wait(&status);

        if ((new_fdw = dup2(fd[0], 0)) < 0) {
            free(cmd_one);
            free(cmd_two);
            return FILE_DESCRIPTOR_DUP_ERROR;
        }

        result = redirect(cmd_one);
        close(fd[0]);
    }

    free(cmd_one);
    free(cmd_two);

    return result;
}

int redirect(char* line) {
    char *str, *str_two, *cmd_one;
    pid_t child_two;
    int p = 0, flag = 1, status, result = 0, fdo, fd, new_fdw;
    char *line_two = strdup(line);
    str = strchr(line, '>');

    if (str != NULL) {
        int n;
        flag = 0;

        p = strlen(str);
        str_two = strdup(str);
        cmd_one = strtok(str, " ");
        n = strlen(str);

        if (n == 1) {
            cmd_one = strtok(NULL, " \n");
            n += strlen(cmd_one) + 1;
        } else {
            cmd_one = cmd_one + 1;
        }

        if ((child_two=fork()) < 0) {
            free(line_two);
            free(str_two);
            return PROCESS_CREATION_ERROR;
        }

        if (child_two == 0) {
            if ((fd = open(cmd_one, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP)) < 0) {
                free(line_two);
                free(str_two);
                return FILE_OPEN_ERROR;
            } else {
                if ((new_fdw=dup2(fd, 1)) < 0) {
                    free(line_two);
                    free(str_two);
                    return FILE_DESCRIPTOR_DUP_ERROR;
                } else {
                    line_two[strlen(line_two) - p] = 0;
                    strcat(line_two, str_two + n);
                    result = redirect(line_two);
                }
                close(fd);
            }
            exit(1);
            
        }
        wait(&status);
        free(str_two);
    } else if ((str = strchr(line, '<')) != NULL) {
        p = strlen(str);
        flag = 0;
        int n;
        str_two = strdup(str);
        cmd_one = strtok(str, " ");
        n = strlen(str);
        
        if (strlen(str) == 1) {
            cmd_one = strtok(NULL, " \n");
            n += strlen(cmd_one) + 1;
        } else {
            cmd_one = cmd_one + 1;
        }

        if ((child_two = fork()) < 0) {
            free(line_two);
            free(str_two);
            return PROCESS_CREATION_ERROR;
        }

        if (child_two == 0) {
            if ((fdo=open(cmd_one, O_RDONLY)) < 0) {
                free(line_two);
                free(str_two);
                return FILE_OPEN_ERROR;
            } else {
                if ((new_fdw=dup2(fdo, 0)) < 0) {
                    return FILE_DESCRIPTOR_DUP_ERROR;
                } else {
                    line[strlen(line_two) - p] = 0;
                    strcat(line_two, str_two + n);
                    result = redirect(line);
                }
            }
            close(fdo);
            exit(1);
        }
        wait(&status);
    }
    if (flag == 1) {
        result = parse_command(line);
        if (result == 2) {
            if ((result=system(line_two)) != -1) {
                result = 0;
            }
        }
    }

    return result;
}

int parse_command(char* line) {
    int status = 0;
    int argc = 0;
    char *argv[ARGUMENT_NUMBER];
    char *part;

    part = strtok(line, " \n\t\r");
    while (part != NULL) {
        argv[argc] = strdup(part);
        argc++;
        part = strtok(NULL, " \n\t\r");
    }

    status = exec_command(argc, argv);
    return status;
}

int exec_command(int argc, char* argv[]) {
    int status = 2;
    char *cmd = strdup(argv[0]);

    if (argc >= 2) {
        if (strcmp(argv[1], "--help") == 0) {
            //print help
            free(cmd);
            return 0;
        }
    }

    if (strcmp(cmd, "ls") == 0) status = ls(argc, argv);
    else if (strcmp(cmd, "cd") == 0) status = cd(argc, argv);
    else if (strcmp(cmd, "cp") == 0) status = cp(argc, argv);
    else if (strcmp(cmd, "dirname") == 0) status = dirname(argc, argv);
    else {
        printf("Command not found");
    }

    free(cmd);
    return status;
}