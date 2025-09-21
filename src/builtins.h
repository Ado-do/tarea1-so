#pragma once

extern char *builtin_str[];
extern int (*builtin_func[])(char **);

int num_builtins(void);
int mi_shell_cd(char **args);
int mi_shell_exit(char **args);
