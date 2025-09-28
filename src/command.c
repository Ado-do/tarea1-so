#include "command.h"

#include <stdlib.h>

struct cmd *create_execcmd() {
    struct execcmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    cmd->type = EXEC;
    return (struct cmd *)cmd;
}

struct cmd *create_redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd) {
    struct redircmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    cmd->type = REDIR;
    cmd->cmd = subcmd;
    cmd->file = file;
    cmd->efile = efile;
    cmd->mode = mode;
    cmd->fd = fd;
    return (struct cmd *)cmd;
}

struct cmd *create_pipecmd(struct cmd *left, struct cmd *right) {
    struct pipecmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    cmd->type = PIPE;
    cmd->left = left;
    cmd->right = right;
    return (struct cmd *)cmd;
}

struct cmd *create_listcmd(struct cmd *left, struct cmd *right) {
    struct listcmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    cmd->type = LIST;
    cmd->left = left;
    cmd->right = right;
    return (struct cmd *)cmd;
}

struct cmd *create_backcmd(struct cmd *subcmd) {
    struct backcmd *cmd;
    cmd = calloc(1, sizeof(*cmd));
    cmd->type = BACK;
    cmd->cmd = subcmd;
    return (struct cmd *)cmd;
}

// nulterminate: agrega caracter null '\0' recursivamente a strings desde el
// nodo cmd
struct cmd *nulterminate(struct cmd *cmd) {
    if (cmd == NULL)
        return 0;

    struct backcmd *bcmd;
    struct execcmd *ecmd;
    struct listcmd *lcmd;
    struct pipecmd *pcmd;
    struct redircmd *rcmd;

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

void free_cmd(struct cmd *cmd) {
    if (cmd == NULL) return;

    union {
        struct execcmd *exec;
        struct redircmd *redir;
        struct pipecmd *pipe;
        struct listcmd *list;
        struct backcmd *back;
    } ucmd;
    ucmd.exec = (struct execcmd *)cmd;

    switch (cmd->type) {
    case EXEC:
        free(ucmd.exec);
        break;
    case REDIR:
        free_cmd(ucmd.redir->cmd);
        free(ucmd.redir);
        break;
    case PIPE:
        free_cmd(ucmd.pipe->left);
        free_cmd(ucmd.pipe->right);
        free(ucmd.pipe);
        break;
    case LIST:
        free_cmd(ucmd.list->left);
        free_cmd(ucmd.list->right);
        free(ucmd.list);
        break;
    case BACK:
        free_cmd(ucmd.back->cmd);
        free(ucmd.back);
        break;
    default:
        free(cmd);
        break;
    }
}
