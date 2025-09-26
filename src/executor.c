#include "executor.h"

#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// runcmd: recorrer arból recursivamente y manejar cada tipo de comando
void runcmd(struct cmd *cmd) {
    int p[2];
    struct backcmd *bcmd;
    struct execcmd *ecmd;
    struct listcmd *lcmd;
    struct pipecmd *pcmd;
    struct redircmd *rcmd;

    if (cmd == NULL)
        exit(EXIT_FAILURE);

    switch (cmd->type) {
    default:
        panic("runcmd");

    case EXEC:
        ecmd = (struct execcmd *)cmd;
        if (ecmd->argv[0] == NULL)
            exit(EXIT_FAILURE);

        execvp(ecmd->argv[0], ecmd->argv);
        fprintf(stderr, "exec %s failed\n", ecmd->argv[0]);
        break;

    case REDIR:
        rcmd = (struct redircmd *)cmd;
        close(rcmd->fd);
        if (open(rcmd->file, rcmd->mode) < 0) {
            exit(EXIT_FAILURE);
        }
        runcmd(rcmd->cmd);
        break;

    case LIST:
        lcmd = (struct listcmd *)cmd;
        if (myfork() == 0)
            runcmd(lcmd->left);

        wait(NULL);
        runcmd(lcmd->right);
        break;

    case PIPE:
        pcmd = (struct pipecmd *)cmd;
        if (pipe(p) < 0)
            panic("pipe");

        if (myfork() == 0) {
            close(STDOUT_FILENO);
            dup(p[1]);
            close(p[0]);
            close(p[1]);
            runcmd(pcmd->left);
        }
        if (myfork() == 0) {
            close(STDIN_FILENO);
            dup(p[0]);
            close(p[0]);
            close(p[1]);
            runcmd(pcmd->right);
        }
        close(p[0]);
        close(p[1]);
        wait(NULL);
        wait(NULL);
        break;

    case BACK:
        bcmd = (struct backcmd *)cmd;
        if (myfork() == 0)
            runcmd(bcmd->cmd);
        break;
    }

    exit(EXIT_SUCCESS);
}
