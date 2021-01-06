#include "mv.h"
#include "cp.h"

#define BUFFER_SIZE 4096

int mv(int argc, char *argv[]) {
    printf("Moving...\n");
    char *targetDirectory;
    char *suffix;
    bool flagI = 0, flagT = 0, flagS = 0, noMvFlag = 1, flagB = 0;
    
    if (strcmp(argv[1], "-h") == 0) {
        //print help
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) { flagI = true; noMvFlag = false; }
        if (strcmp(argv[i], "-t") == 0 && i+1 < argc) { 
            flagT = true; 
            noMvFlag = false; 
            targetDirectory = (char*)malloc(strlen(argv[i + 1] + 1));
            strcpy(targetDirectory, argv[i + 1]);
        }
        if (strcmp(argv[i], "-b") == 0) { flagB = true; noMvFlag = false; }
        if (strcmp(argv[i], "-S") == 0 && flagB && i + 1 < argc) { 
            flagS = true; 
            noMvFlag = false; 
            suffix = (char*)malloc(strlen(argv[i + 1]) + 1);
            strcpy(suffix, argv[i + 1]);
        }
    }

    const char* source = argv[argc - 2];
    const char* destination = argv[argc - 1];

    if (isDirectory(source)) {
        if (flagT) {
            char* newDestination = (char*)malloc(sizeof(destination) + strlen(targetDirectory) + 2);
            strcpy(newDestination, destination);
            strcat(newDestination, "/");
            strcat(newDestination, targetDirectory);
            mv_recursive(source, newDestination, flagI);
        }
        mv_recursive(source, destination, flagI);
    } else {
        printf("Started moving\n");
        int fd[2];

        if (flagI) {
            printf("Do you want to move %s to %s? y/n", source, destination);
            char answer;
            scanf("%c", &answer);
            if (answer != 'y') {
                return 0;
            }
        }

        fd[0] = open(source, O_RDONLY);
        fd[1] = open(destination, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

        if (fd[0] == -1) {
            printf("Error opening file %s\n", source);
            return ERROR_OPENING_SOURCE;
        }

        if (fd[1] == -1) {
            printf("Error opening file %s\n", destination);
            return ERROR_OPENING_DESTINATION;
        }

        int status = mv_simple(fd[0], fd[1]);
    
        if (status == 0) {
            printf("Copied successfully!\n");
            remove(source);
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
    }
}

int mv_simple(int source, int destination) {
    int nbread, nbwrite;
    char *buffer[BUFFER_SIZE];

    while ((nbread = read(source, buffer, BUFFER_SIZE)) > 0) {
        if (write(destination, buffer, nbread) != nbread) {
            printf("Error writing to destination\n");
            return ERROR_WRITING_TO_DEST;
        }
    }

    if (nbread == -1) {
        return ERROR_READING_FROM_SOURCE;
    }

    if (close(source) == -1) {
        return ERROR_CLOSING_SOURCE;
    }

    if (close(destination) == -1) {
        return ERROR_CLOSING_DESTINATION;
    }

    return 0;
}

int mv_recursive(const char* source, const char* destination, bool flagI) {
    DIR *current_directory = opendir(source);
    struct dirent *de;
    int result = 0;
    int dir_creation_status;
    char *path;
    struct stat *buffer2 = (struct stat*)malloc(sizeof(struct stat));
    lstat(destination, buffer2);
    if (access(destination, F_OK)) {
        dir_creation_status = mkdir(destination, 0777);
        if (dir_creation_status < 0) {
            printf("Error creating destination folder %s.\nErr code: %d\n", destination, dir_creation_status);
            closedir(current_directory);
            free(buffer2);
            free(path);

            return CANNOT_CREATE_DIRECTORY;
        }
    }

    path = strdup(source);
    while (de = readdir(current_directory)) {
        if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..") && de) {
            struct stat *buffer = (struct stat*)malloc(sizeof(struct stat));
            path = (char*)realloc(path, strlen(source) + strlen(de->d_name) + 2);
            strcpy(path, source);
            strcat(path, "/");
            strcat(path, de->d_name);
            lstat(path, buffer);
            if (S_ISDIR(buffer->st_mode)) {
                char *dest2 = (char*)malloc(sizeof(char) * strlen(destination) + strlen(path) - strlen(source) + 2);
                strcpy(dest2, destination);
                strcat(dest2, path + strlen(source));
                if (access(dest2, F_OK)) {
                    if (mkdir(dest2, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP) < 0) {
                        printf("Error creating folder %s.\n", dest2);
                        closedir(current_directory);
                        free(buffer);
                        free(buffer2);
                        free(path);
                        free(dest2);
                        return CANNOT_CREATE_DIRECTORY;
                    }
                }
                printf("Created folder %s\n", destination);

                result = mv_recursive(path, dest2, flagI);

                free(dest2);
            }

            if (S_ISREG(buffer->st_mode)) {
                
                char *dest2 = (char*)malloc(sizeof(char) * strlen(destination) + strlen(path) - strlen(source) + 2);
                if (flagI) {
                    printf("Do you want to move %s to %s? y/n", path, dest2);
                    char answer;
                    scanf("%c", &answer);
                    if (answer != 'y') {
                        continue;
                    }
                }
                strcpy(dest2, destination);
                strcat(dest2, path + strlen(source));
                printf("Created file %s\n", dest2);

                int fd[2];

                fd[0] = open(path, O_RDONLY);
                fd[1] = open(dest2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

                result = mv_simple(fd[0], fd[1]);
                if (result == 0) {
                    remove(path);
                }
                printf("Result for %s is %d\n", dest2, result);
                free(dest2);
            }
            free(buffer);
        }
    }
    rmdir(source);
    closedir(current_directory);
    free(path);
    free(buffer2);
    return result;
}