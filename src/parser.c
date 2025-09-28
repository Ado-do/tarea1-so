#include "parser.h"

/**
Gramatica utilizada:
    LINE    -> PIPE &*
            -> PIPE &* ; LINE

    PIPE    -> EXEC
            -> EXEC | PIPE

    EXEC    -> REDIR {aaa REDIR}*
            -> ( BLOCK )

    REDIR   -> > aaa
            -> < aaa
            -> >> aaa
            -> e

    BLOCK   -> ( LINE ) REDIR

en donde,
    aaa : programa
    *   : 0 o más ocurrencias
    {}  : agrupa terminales
    e   : terminal vació
*/

#include "command.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const char *WHITESPACE = " \t\r\n\v";
const char *SYMBOLS = "(;|><&;)";

// Declaraciónes adelantadas (para permitir su uso)
struct cmd *parse_line(char **, char *);
struct cmd *parse_pipe(char **, char *);
struct cmd *parse_exec(char **, char *);
struct cmd *parse_block(char **, char *);
struct cmd *parse_redirs(struct cmd *, char **, char *);
int gettoken(char **, char *, char **, char **);
int peek(char **, char *, char *);

// gettoken: extraer token desde string en *ps.
//  Si ptoken y ptoken_end != NULL, entonces guardar inicio y final del token
//  guardados en ellos. Retorna tipo de token. ('a': prog o arg, '+': >>)
int gettoken(char **ps, char *ps_end, char **ptok, char **ptok_end) {
    char *s = *ps;
    while (s < ps_end && strchr(WHITESPACE, *s))
        s++;

    if (ptok)
        *ptok = s;

    int tok_type = *s;

    switch (*s) {
    case '\0':
        break;

    case '|':
    case '(':
    case ')':
    case ';':
    case '&':
    case '<':
        s++;
        break;

    case '>':
        s++;
        if (*s == '>') {
            tok_type = '+';
            s++;
        }
        break;

    default:
        tok_type = 'a';
        while (s < ps_end && !strchr(WHITESPACE, *s) && !strchr(SYMBOLS, *s))
            s++;
        break;
    }

    if (ptok_end)
        *ptok_end = s;

    while (s < ps_end && strchr(WHITESPACE, *s))
        s++;

    *ps = s;

    return tok_type;
}

// peek: consulta si el siguiente token pertenece a token_set
int peek(char **ps, char *ps_end, char *tok_set) {
    char *s = *ps;
    while (s < ps_end && strchr(WHITESPACE, *s))
        s++;
    *ps = s;
    return (*s && strchr(tok_set, *s));
}

// parse_cmd: parsea string del comando según gramática definida y retorna nodo padre del árbol
struct cmd *parse_cmd(char *s) {
    char *s_end = s + strlen(s);
    struct cmd *cmd = parse_line(&s, s_end);

    while (s < s_end && strchr(WHITESPACE, *s))
        s++;

    if (s != s_end) {
        fprintf(stderr, "leftovers: %s\n", s);
        panic("syntax");
    }

    nulterminate(cmd);

    return cmd;
}

// parse_line: parsea según variable LINE de la gramática
struct cmd *parse_line(char **ps, char *ps_end) {
    struct cmd *cmd = parse_pipe(ps, ps_end);

    while (peek(ps, ps_end, "&")) {
        gettoken(ps, ps_end, NULL, NULL);
        cmd = create_backcmd(cmd);
    }

    if (peek(ps, ps_end, ";")) {
        gettoken(ps, ps_end, NULL, NULL);
        cmd = create_listcmd(cmd, parse_line(ps, ps_end));
    }

    return cmd;
}

// parse_line: parsea según variable PIPE de la gramática
struct cmd *parse_pipe(char **ps, char *ps_end) {
    struct cmd *cmd = parse_exec(ps, ps_end);

    if (peek(ps, ps_end, "|")) {
        gettoken(ps, ps_end, NULL, NULL);
        cmd = create_pipecmd(cmd, parse_pipe(ps, ps_end));
    }

    return cmd;
}

// parse_block: parsea según variable BLOCK de la gramática
struct cmd *parse_block(char **ps, char *ps_end) {
    if (!peek(ps, ps_end, "("))
        panic("parseblock");

    gettoken(ps, ps_end, NULL, NULL);
    struct cmd *cmd = parse_line(ps, ps_end);

    if (!peek(ps, ps_end, ")"))
        panic("syntax - missing )");

    gettoken(ps, ps_end, NULL, NULL);
    cmd = parse_redirs(cmd, ps, ps_end);

    return cmd;
}

// parse_exec: parsea según variable EXEC de la gramática
struct cmd *parse_exec(char **ps, char *ps_end) {
    if (peek(ps, ps_end, "("))
        return parse_block(ps, ps_end);

    struct cmd *ret = create_execcmd(); // alloc execmd
    struct execcmd *cmd = (struct execcmd *)ret;

    ret = parse_redirs(ret, ps, ps_end);

    int argc = 0;
    int tok_type;
    char *ptok, *ptok_end;
    while (!peek(ps, ps_end, "|)&;")) {
        if ((tok_type = gettoken(ps, ps_end, &ptok, &ptok_end)) == '\0')
            break;

        if (tok_type != 'a')
            panic("syntax");

        cmd->argv[argc] = ptok;
        cmd->eargv[argc] = ptok_end;
        argc++;

        if (argc >= MAXARGS)
            panic("too many args");

        ret = parse_redirs(ret, ps, ps_end);
    }

    cmd->argv[argc] = NULL;
    cmd->eargv[argc] = NULL;
    return ret;
}

// parse_redirs: parsea según variable REDIR de la gramática
struct cmd *parse_redirs(struct cmd *cmd, char **ps, char *ps_end) {
    while (peek(ps, ps_end, "<>")) {
        int tok_type = gettoken(ps, ps_end, NULL, NULL);

        char *filepath, *filepath_end;
        if (gettoken(ps, ps_end, &filepath, &filepath_end) != 'a')
            panic("missing file for redirection");

        int mode;
        switch (tok_type) {
        case '<':
            mode = O_RDONLY;
            cmd = create_redircmd(cmd, filepath, filepath_end, mode, STDIN_FILENO);
            break;
        case '>':
            mode = O_WRONLY | O_CREAT | O_TRUNC;
            cmd = create_redircmd(cmd, filepath, filepath_end, mode, STDOUT_FILENO);
            break;
        case '+': // >>
            mode = O_WRONLY | O_CREAT | O_APPEND;
            cmd = create_redircmd(cmd, filepath, filepath_end, mode, STDOUT_FILENO);
            break;
        default:
            panic("parse_redirs");
            break;
        }
    }

    return cmd;
}
