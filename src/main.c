#include "builtin.h"
#include "executor.h"
#include "parser.h"
#include "prompt.h"
#include "utils.h"

#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BANNER_FILE "src/banner.txt"

void print_banner() {
    if (myfork() == 0) {
        execvp("cat", (char *[]){"cat", BANNER_FILE, NULL});
    }
    wait(NULL);
}

void init_readline() {
    using_history();
    rl_bind_key(TAB, rl_complete);
}

int main() {
    char *cmd_buf;
    int child_exit_status;

    init_readline();
    print_banner();

    while ((cmd_buf = readline(get_prompt())) != NULL) {
        if (strlen(cmd_buf))
            add_history(cmd_buf);

        struct cmd *cmd = parse_cmd(cmd_buf);

        if (cmd->type == EXEC) {
            struct execcmd *ecmd = (struct execcmd *)cmd;
            char *name = ecmd->argv[0];
            if (name && is_builtin(name)) {
                execute_builtin(ecmd);
                free_cmd(cmd);
                continue;
            }
        }

        if (myfork() == 0) {
            // child process
            runcmd(cmd);
        }

        // parent process
        wait(&child_exit_status);

        free_cmd(cmd);
        free(cmd_buf);
    }

    return 0;
}
