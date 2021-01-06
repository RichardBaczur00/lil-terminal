#include "dirname.h"

int dirname(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    printf("%s\n", dirname(argv[1]));

    return 0;
}