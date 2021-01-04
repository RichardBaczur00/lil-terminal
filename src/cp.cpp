#include "cp.h"

#define BUFFER_SIZE 4096

bool flagI=0, flagR=0, flagT=0, flagV=0, noCpFlag=1;
int cp(int argc, char *argv[]) {
    int fd[2];

    if (strcmp(argv[1], "-h") == 0) {
        //print help
        return 0;
    }

    //check params
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) { flagI = true; noCpFlag = false; }
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "-R") == 0) { flagR = true; noCpFlag = false; }
        if (strcmp(argv[i], "-t") == 0) { flagT = true; noCpFlag = false; }
        if (strcmp(argv[i], "-v") == 0) { flagV = true; noCpFlag = false; }
    }

    //open file descriptors
    fd[0] = open(argv[argc - 2], O_RDONLY);
    fd[1] = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    //check file descriptors
    if (fd[0] == -1) {
        printf("Error opening file %s (source)\n", argv[argc - 2]);
        return ERROR_OPENING_SOURCE;
    }

    if (fd[1] == -1) {
        printf("Error opening file %s (destination)\n", argv[argc - 1]);
        return ERROR_OPENING_DESTINATION;
    }

    //check flags and execute accordingly
    if (noCpFlag) {
        // copy and get operation status
        int status = simpleCopy(fd[0], fd[1]);
        
        if (status == 0) {
            return 0;
        } else {
            if (status == ERROR_WRITING_TO_DEST) {
                printf("Error writing to %s\n", argv[argc - 1]);
            } else if (status == ERROR_READING_FROM_SOURCE) {
                printf("Error reading from %s\n", argv[argc - 2]);
            } else if (status == ERROR_CLOSING_DESTINATION) {
                printf("Error closing destination\n");
            } else if (status == ERROR_CLOSING_SOURCE) {
                printf("Error closing source\n");
            }
        }
        
    } else if (flagI) {
        //check if the destination file exists
        if (access(argv[argc - 1], F_OK) == 0) {
            //file exists, prompt user
            char answer;
            printf("File %s already exists, do you want to overwrite it? y/n: ");
            printf("\n");
            scanf("%c", &answer);
            //if the answer is 'y' proceed
            if (answer == 'y') {
                simpleCopy(fd[0], fd[1]);
            } else {
                //skip file if the answer is anything else
                printf("Skipping file...\n");
            }
        }
    } else if (flagR) {
        //if the flag is set and the source is not a directory, stop execution
        if (isDirectory(argv[argc - 2])) {
            //if it is create the destination directory...
            mkdir(argv[argc - 1], 0777);
            //... and start copying
            recursiveCopy(argv[argc - 2], argv[argc - 1]);
        } else {
            return SOURCE_NOT_A_DIRECTORY;
        }
    }
}

int simpleCopy(int source, int destination) {
    int nbread, nbwrite;
    char *buffer[BUFFER_SIZE];

    //write from source to buffer and from buffer to destination
    while ((nbread = read(source, buffer, BUFFER_SIZE)) > 0) {
        if (write(destination, buffer, nbread) != nbread) {
            return ERROR_WRITING_TO_DEST;
        }
    }

    //error while reading
    if (nbread == -1) {
        return ERROR_READING_FROM_SOURCE;
    }

    //error while closing source file descriptor
    if (close(source) == -1) {
        return ERROR_CLOSING_SOURCE;
    }

    //error while closing destination file descriptor
    if (close(destination) == -1) {
        return ERROR_CLOSING_DESTINATION;
    }

    return 0;
}

bool isDirectory(const char* path) {
    struct stat st;
    //check if a file is a directory or a regular file
    if (stat(path, &st) != 0) {
        return false;
    }
    return (S_ISDIR(st.st_mode) > 0) ? true : false;
}

int recursiveCopy(const char* source, const char* destination) {
    //implementation of recursive traversal and copy
    DIR *currentDirectory = opendir(source);
    struct dirent *de;
    int result = 0;
    char *path;
    struct stat *buffer2 = (struct stat*)malloc(sizeof(struct stat));
    lstat(destination, buffer2);
    if (access(destination, F_OK))
        if (mkdir(destination, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP) < 0) {
            printf("Cannot create destination folder\n");
            closedir(currentDirectory);
            free(buffer2);
            free(path);

            return CANNOT_CREATE_DIRECTORY;
        }
    path = strdup(source);
    while (de = readdir(currentDirectory)) {
        if (strcmp(de->d_name, ".")&&strcmp(de->d_name, "..") && de) {
            struct stat *buffer = (struct stat*)malloc(sizeof(struct stat));
            path = (char*)realloc(path, strlen(source) + strlen(de->d_name) + 2);
            strcpy(path, source);
            strcat(path, "/");
            strcat(path, de->d_name);
            lstat(path, buffer);
            if (S_ISDIR(buffer->st_mode)) {
                char *dest2 = (char*)malloc(sizeof(char)*strlen(destination) + strlen(path) - strlen(source) + 2);
                strcpy(dest2, destination);
                strcat(dest2, path + strlen(source));
                if (access(dest2, F_OK)) {
                    if (mkdir(dest2, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP) < 0) {
                        printf("Cannot create folder %s\n", dest2);
                        closedir(currentDirectory);
                        free(buffer);
                        free(buffer2);
                        free(path);
                        free(dest2);
                        return CANNOT_CREATE_DIRECTORY;
                    }
                    printf("Created folder %s", dest2);
                    result = recursiveCopy(path, dest2);

                    free(dest2);
                }
            }
            if (S_ISREG(buffer->st_mode)) {
                char *dest2 = (char*)malloc(sizeof(char) * strlen(destination) + strlen(path) - strlen(source) + 2);
                strcpy(dest2, destination);
                strcat(dest2, path + strlen(source));
                printf("Created file %s", dest2);

                int fd[2];

                fd[0] = open(path, O_RDONLY);
                fd[1] = open(dest2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

                result = simpleCopy(fd[0], fd[1]);
                printf("Result for %s is %d", dest2, result);
                free(dest2);
            }

            free(buffer);
        }
    }

    closedir(currentDirectory);
    free(path);
    free(buffer2);
    return result;
}