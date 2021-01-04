#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NMAX 100
#define MAX_TOK 30
#define TOO_MANY_ARGUMENTS_ERROR 0x001
#define TOO_FEW_ARGUMENTS_ERROR 0x010
#define CANNOT_ACCESS_ERROR 0x100

int cd(int argc, char *argv[]);