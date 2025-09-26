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

void print_banner() {
    if (fork() == 0) {
        execvp("cat", (char *[]){"cat", "banner.txt", NULL});
    }
    wait(NULL);
}

int main() {
    char *cmd_buf;
    int status;

    print_banner();

    while ((cmd_buf = readline(get_prompt(SHELL_NAME))) != NULL) {
        if (strlen(cmd_buf))
            add_history(cmd_buf);

        if (is_builtin(cmd_buf)) {
            handle_builtin(cmd_buf);
        } else {
            // child process
            if (fork() == 0) {
                // parse command
                struct cmd *cmd = parse_cmd(cmd_buf);
                // execute command
                runcmd(cmd);
            }
            // parent process
            wait(&status);
        }
        free(cmd_buf);
    }

    return 0;
}
