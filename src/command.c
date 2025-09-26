#include "command.h"

#include <stdlib.h>

struct cmd *execcmd(void) {
    struct execcmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    // cmd = malloc(sizeof(*cmd));
    // memset(cmd, 0, sizeof(*cmd));
    cmd->type = EXEC;
    return (struct cmd *)cmd;
}

struct cmd *redircmd(struct cmd *subcmd, char *file, char *efile, int mode,
                     int fd) {
    struct redircmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    // cmd = malloc(sizeof(*cmd));
    // memset(cmd, 0, sizeof(*cmd));
    cmd->type = REDIR;
    cmd->cmd = subcmd;
    cmd->file = file;
    cmd->efile = efile;
    cmd->mode = mode;
    cmd->fd = fd;
    return (struct cmd *)cmd;
}

struct cmd *pipecmd(struct cmd *left, struct cmd *right) {
    struct pipecmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    // cmd = malloc(sizeof(*cmd));
    // memset(cmd, 0, sizeof(*cmd));
    cmd->type = PIPE;
    cmd->left = left;
    cmd->right = right;
    return (struct cmd *)cmd;
}

struct cmd *listcmd(struct cmd *left, struct cmd *right) {
    struct listcmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    // cmd = malloc(sizeof(*cmd));
    // memset(cmd, 0, sizeof(*cmd));
    cmd->type = LIST;
    cmd->left = left;
    cmd->right = right;
    return (struct cmd *)cmd;
}

struct cmd *backcmd(struct cmd *subcmd) {
    struct backcmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    // cmd = malloc(sizeof(*cmd));
    // memset(cmd, 0, sizeof(*cmd));
    cmd->type = BACK;
    cmd->cmd = subcmd;
    return (struct cmd *)cmd;
}

// nulterminate: agrega caracter null '\0' recursivamente a strings desde el nodo cmd
struct cmd *nulterminate(struct cmd *cmd) {
    struct backcmd *bcmd;
    struct execcmd *ecmd;
    struct listcmd *lcmd;
    struct pipecmd *pcmd;
    struct redircmd *rcmd;

    if (cmd == NULL)
        return 0;

    switch (cmd->type) {
    case EXEC:
        ecmd = (struct execcmd *)cmd;
        for (int i = 0; ecmd->argv[i]; i++)
            *ecmd->eargv[i] = '\0';
        break;

    case REDIR:
        rcmd = (struct redircmd *)cmd;
        nulterminate(rcmd->cmd);
        *rcmd->efile = '\0';
        break;

    case PIPE:
        pcmd = (struct pipecmd *)cmd;
        nulterminate(pcmd->left);
        nulterminate(pcmd->right);
        break;

    case LIST:
        lcmd = (struct listcmd *)cmd;
        nulterminate(lcmd->left);
        nulterminate(lcmd->right);
        break;

    case BACK:
        bcmd = (struct backcmd *)cmd;
        nulterminate(bcmd->cmd);
        break;
    }
    return cmd;
}
