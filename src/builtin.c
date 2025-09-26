#include "builtin.h"

#include "stdio.h"

int is_builtin(char *cmd) {
    return 0;
}

int handle_builtin(char *cmd) {
    printf("builtin command: %s\n", cmd);
    return 0;
}
