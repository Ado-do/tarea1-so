#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *DELIM = " \t\r\n\a";

char *read_line(void) {
    char *line = NULL;
    size_t bufsize = 0; // Usado por getline
    getline(&line, &bufsize, stdin);
    return line;
}

char **split_line(char *line) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "mi_shell: error de asignacion\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIM);
    while (token != NULL) {
        printf("Token extraido: %s\n", token);
        tokens[position] = token;
        position++;
        // Reasignar memoria si es necesario
        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "mi_shell: error de reasignacion\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
