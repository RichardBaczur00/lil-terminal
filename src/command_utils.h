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

int exec_command(int, char *argv[]);
int parse_command(char*);
int exec_simple(char*);
int redirect(char*);
int double_redirect(char*);
