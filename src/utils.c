#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PANIC_BUFFER 200

// get_formated_string: formatea string y la retorna
const char *get_fstr(const char *s, ...) {
    static char buffer[MAX_PANIC_BUFFER];

    va_list args;
    va_start(args, s);
    int required_byte_count = vsnprintf(buffer, MAX_PANIC_BUFFER, s, args);
    va_end(args);

    // Check for errors in vsnprintf
    if (required_byte_count < 0) {
        fprintf(stderr, "Error formatting panic message.\n");
        exit(EXIT_FAILURE);
    }

    // Handle truncation
    if (required_byte_count >= MAX_PANIC_BUFFER) {
        snprintf(buffer + MAX_PANIC_BUFFER - 4, 4, "...");
    }

    // Print the formatted message
    fprintf(stderr, "%s\n", buffer);
    exit(EXIT_FAILURE);
}

// panic: muestra error y termina proceso
void panic(const char *s) {
    fprintf(stderr, "%s\n", s);
    exit(EXIT_FAILURE);
}

// myfork: wrapper de fork() para manejar errores
int myfork() {
    int pid = fork();
    if (pid == -1)
        perror("fork");
    return pid;
}

// get_argc: get arguments count
int get_argc(char **argv) {
    int cnt = 0;
    while (argv[cnt] != NULL)
        cnt++;
    return cnt;
}
