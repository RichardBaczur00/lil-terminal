#include "dirname.h"

int dirname(int argc, char *argv[]) {
    const char dot[] = ".";
    char *last_slash;

    if (argc != 2) {
        printf("Incorect number of arguments!\n");
        return -1;
    }

    last_slash = argv[1] != NULL ? strrchr(argv[1], '/') : NULL;
    if (last_slash != NULL && last_slash != argv[1] && last_slash[1] == '\0') {
        char *runp;
        for (runp = last_slash; runp != argv[1]; --runp) {
            if (runp[-1] != '/') {
                break;
            }
        }

        if (runp != argv[1]) {
            last_slash = (char*)memrchr(argv[1], '/', runp - argv[1]);
        }
    }

    if (last_slash != NULL) {
        char *runp;

        for (runp = last_slash; runp != argv[1]; --runp) {
            if (runp[-1] != '/') {
                break;
            }
        }

        if (runp == argv[1]) {
            if (last_slash == argv[1] + 1) {
                ++last_slash;
            } else {
                last_slash = argv[1] + 1;
            }
        } else {
            last_slash = runp;
        }
    } else {
        argv[1] = (char*) dot;
    }

    printf("Result of dirname is: %s\n", argv[1]);

    free(last_slash);

    return 0;
}