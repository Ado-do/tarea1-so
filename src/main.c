#include "prompt.h"

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHELL_NAME "mish"


int is_builtin(char *cmd);
int handle_builtin(char *cmd);

// read -> eval -> print -> loop
int main() {
    char *buf;
    int status;

    // READ
    while ((buf = readline(get_prompt(SHELL_NAME))) != NULL) {
        char *cmd = buf;

        if (strlen(cmd)) add_history(cmd);

        // EVAL -> PRINT
        if (is_builtin(cmd)) {
            handle_builtin(cmd);
        } else {
            printf("external command: %s\n", cmd);

            // child process
            // if (fork() == 0) {
                // parse line
                // execute command
            // }

            // parent process
            wait(&status);
        }
        free(buf);
    }

    return 0;
}

int is_builtin(char *cmd) {
    return 1;
}

int handle_builtin(char *cmd) {
    printf("builtin command: %s", cmd);
    return 0;
}
