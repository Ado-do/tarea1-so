#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// panic: muestra error y termina proceso
void panic(char *s) {
    fprintf(stderr, "-mish: %s\n", s);
    exit(EXIT_FAILURE);
}

// myfork: wrapper de fork() para manejar errores
int myfork() {
    int pid = fork();
    if (pid == -1)
        perror("fork");
    return pid;
}
