#include "executor.h"
#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int command_execute(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Estamos en el proceso hijo
        // Se llama a execvp para ejecutar el comando
        if (execvp(args[0], args) == -1) {
            perror("mi_shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error en la creación del fork
        perror("mi_shell");
    } else {
        // Estamos en el proceso padre
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    // Buscamos si la linea ingresada tiene comandos internos como CD
    int n = num_builtins();
    for (int i = 0; i < n; i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return command_execute(args);
}
