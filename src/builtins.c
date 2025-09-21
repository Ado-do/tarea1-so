#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Lista de comandos internos */
char *builtin_str[] = {"cd", "exit"};
int (*builtin_func[])(char **) = {&mi_shell_cd, &mi_shell_exit};

int num_builtins(void) { return sizeof(builtin_str) / sizeof(char *); }

/* Implementaciones de los comandos internos */
int mi_shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mi_shell: se esperaba un argumento para \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("mi_shell");
        }
    }
    return 1;
}

int mi_shell_exit(char **args) { exit(EXIT_SUCCESS); }
