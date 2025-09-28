#pragma once

#include "command.h"

typedef int (*builtin_func)(struct execcmd *);

struct builtin_cmd {
    char *name;
    builtin_func func;
};

int is_builtin(const char *);
int execute_builtin(struct execcmd *);

int builtin_cd(struct execcmd *);
int builtin_pwd(struct execcmd *);
int builtin_exit(struct execcmd *);
int builtin_help(struct execcmd *);
int builtin_history(struct execcmd *);
int builtin_miprof(struct execcmd *);
