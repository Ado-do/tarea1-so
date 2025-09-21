#include "executor.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#define SHELL_NAME "mi_shell"

int main(int argc, char **argv) {
    char *line;
    char **args;
    int status;

    do {
        printf("%s> ", SHELL_NAME); // prompt
        line = read_line();         // Leer la línea del usuario
        args = split_line(line);
        execute(args);
        free(args);
        free(line);
    } while (1);

    return 0;
}
