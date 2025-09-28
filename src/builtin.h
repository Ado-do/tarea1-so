#pragma once

#include "command.h"

typedef int (*builtin_func)(struct execcmd *);

struct builtin_cmd {
    char *name;
    builtin_func func;
};

int is_builtin(struct execcmd *ecmd);
int execute_builtin(struct execcmd *ecmd);

int builtin_cd(struct execcmd *ecmd);
int builtin_pwd(struct execcmd *ecmd);
int builtin_exit(struct execcmd *ecmd);
int builtin_help(struct execcmd *ecmd);
int builtin_history(struct execcmd *ecmd);
