#include "builtin.h"
#include "executor.h"
#include "parser.h"
#include "prompt.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define SHELL_NAME "mish"

// read -> eval -> print -> loop
int main() {
    char *buf;
    int status;

    printf("Bienvenido a la shell mish!\n");

    // READ
    while ((buf = readline(get_prompt(SHELL_NAME))) != NULL) {
        char *cmd_s = buf;

        if (strlen(cmd_s))
            add_history(cmd_s);

        // EVAL -> PRINT
        if (is_builtin(cmd_s)) {
            handle_builtin(cmd_s);
        } else {
            // child process
            if (fork() == 0) {
                // parse line
                struct cmd *cmd = parse_cmd(cmd_s);
                // execute command
                runcmd(cmd);
            }

            // parent process
            wait(&status);
        }
        free(buf);
    }

    return 0;
}
