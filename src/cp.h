#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <fcntl.h>

#define INCORECT_NO_ARGS 0x00000001
#define ERROR_OPENING_SOURCE 0x00000010
#define ERROR_OPENING_DESTINATION 0x00000100
#define ERROR_WRITING_TO_DEST 0x00001000
#define ERROR_READING_FROM_SOURCE 0x00010000
#define ERROR_CLOSING_SOURCE 0x00100000
#define ERROR_CLOSING_DESTINATION 0x01000000
#define SOURCE_NOT_A_DIRECTORY 0x10000000
#define CANNOT_CREATE_DIRECTORY 0x00000011

int cp(int argc, char *argv[]);
int simpleCopy(int source, int destination);
int recursiveCopy(const char*, const char*);
bool isDirectory(const char*);
