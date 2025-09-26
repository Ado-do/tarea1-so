#include "executor.h"

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
            if (ecmd->argv[0] == 0)
                exit(1);
            execvp(ecmd->argv[0], ecmd->argv);
            fprintf(stderr, "exec %s failed\n", ecmd->argv[0]);
            exit(1); // execvp failed

        case REDIR:
            rcmd = (struct redircmd *)current;
            close(rcmd->fd);
            if (open(rcmd->file, rcmd->mode) < 0) {
                fprintf(stderr, "open %s failed\n", rcmd->file);
                exit(1);
            }
            current = rcmd->cmd; // Move to next command instead of recursing
            continue;

        case LIST:
            lcmd = (struct listcmd *)current;
            if (myfork() == 0) {
                current = lcmd->left; // Process left side
                continue;
            }
            wait(0);
            current = lcmd->right; // Then process right side
            continue;

        case PIPE:
            pcmd = (struct pipecmd *)current;
            if (pipe(p) < 0)
                panic("pipe");
            if (myfork() == 0) {
                close(1);
                dup(p[1]);
                close(p[0]);
                close(p[1]);
                current = pcmd->left;
                continue;
            }
            if (myfork() == 0) {
                close(0);
                dup(p[0]);
                close(p[0]);
                close(p[1]);
                current = pcmd->right;
                continue;
            }
            close(p[0]);
            close(p[1]);
            wait(0);
            wait(0);
            current = NULL; // Pipe processing complete
            break;

        case BACK:
            bcmd = (struct backcmd *)current;
            if (myfork() == 0) {
                current = bcmd->cmd;
                continue;
            }
            current = NULL; // Background job launched
            break;

        default:
            panic("runcmd");
        }

        break; // Exit the loop
    }
    exit(0);
}
