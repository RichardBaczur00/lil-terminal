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
#include "ls.h"
#include "cd.h"
#include "cp.h"
#include "dirname.h"

int exec_command(int, char *argv[]);
int parse_command(char*);
int exec_simple(char*);
int redirect(char*);
int double_redirect(char*);
