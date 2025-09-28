#include "executor.h"

#include "builtin.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// runcmd: recorrer arból recursivamente y manejar cada tipo de comando
void runcmd(struct cmd *cmd) {
    struct cmd *current = cmd;

    while (current != NULL) {
        int p[2];
        struct backcmd *bcmd;
        struct execcmd *ecmd;
        struct listcmd *lcmd;
        struct pipecmd *pcmd;
        struct redircmd *rcmd;

        switch (current->type) {
        case EXEC:
            ecmd = (struct execcmd *)current;
            if (ecmd->argv[0] == NULL)
                exit(EXIT_FAILURE);

            if (is_builtin(ecmd->argv[0])) {
                execute_builtin(ecmd);
                exit(EXIT_SUCCESS);
            }
            if (execvp(ecmd->argv[0], ecmd->argv) == -1) {
                fprintf(stderr, "%s: command not found...\n", ecmd->argv[0]);
            }
            break;

        case REDIR:
            rcmd = (struct redircmd *)current;
            close(rcmd->fd);

            int file_permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            if (open(rcmd->file, rcmd->mode, file_permissions) < 0)
                perror("runcmd redir");

            current = rcmd->cmd;
            continue;

        case LIST:
            lcmd = (struct listcmd *)current;
            if (myfork() == 0) {
                current = lcmd->left;
                continue;
            }
            wait(NULL);
            current = lcmd->right;
            continue;

        case PIPE:
            pcmd = (struct pipecmd *)current;
            if (pipe(p) < 0)
                perror("pipe");

            if (myfork() == 0) {
                close(STDOUT_FILENO);
                dup(p[1]);
                close(p[0]);
                close(p[1]);
                current = pcmd->left;
                continue;
            }
            if (myfork() == 0) {
                close(STDIN_FILENO);
                dup(p[0]);
                close(p[0]);
                close(p[1]);
                current = pcmd->right;
                continue;
            }

            close(p[0]);
            close(p[1]);
            wait(NULL);
            wait(NULL);
            current = NULL;
            break;

        case BACK:
            bcmd = (struct backcmd *)current;
            if (myfork() == 0) {
                current = bcmd->cmd;
                continue;
            }
            current = NULL;
            break;

        default:
            panic("runcmd");
        }

        break;
    }

    exit(EXIT_SUCCESS);
}
