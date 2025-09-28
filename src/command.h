#pragma once

typedef enum { EXEC, REDIR, PIPE, LIST, BACK } cmd_type;

#define MAXARGS 20

// nodo genérico del árbol de representación
struct cmd {
    cmd_type type;
};

// nodo exec: representa llamada a programa (prog args ...)
struct execcmd {
    cmd_type type;
    char *argv[MAXARGS];
    char *eargv[MAXARGS];
};

// nodo redir: representa re-dirección de I/O (<, >, >>)
struct redircmd {
    cmd_type type;
    struct cmd *cmd;
    char *file;
    char *efile;
    int mode;
    int fd;
};

// nodo pipe: representa pipes de comandos (cmd | cmd)
struct pipecmd {
    cmd_type type;
    struct cmd *left;
    struct cmd *right;
};

// nodo list: representa listas de comandos (cmd ; cmd)
struct listcmd {
    cmd_type type;
    struct cmd *left;
    struct cmd *right;
};

// nodo background: representa comando ejecutado en background (cmd &)
struct backcmd {
    cmd_type type;
    struct cmd *cmd;
};

struct cmd *create_execcmd();
struct cmd *create_redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd);
struct cmd *create_pipecmd(struct cmd *left, struct cmd *right);
struct cmd *create_listcmd(struct cmd *left, struct cmd *right);
struct cmd *create_backcmd(struct cmd *subcmd);

void free_cmd(struct cmd *cmd);

struct cmd *nulterminate(struct cmd *cmd);
