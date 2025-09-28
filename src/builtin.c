#include "builtin.h"

#include "command.h"

#include "stdio.h"
#include "utils.h"
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct builtin_cmd builtin[] = {
    {"cd", builtin_cd},     {"pwd", builtin_pwd},         {"exit", builtin_exit},
    {"help", builtin_help}, {"history", builtin_history},
};
static const int nbuiltins = sizeof(builtin) / sizeof(*builtin);

// is_builtin: comprobar si es comando builtin de la shell
int is_builtin(struct execcmd *ecmd) {
    char *cmdname = ecmd->argv[0];
    if (cmdname == NULL)
        return 0;

    for (int i = 0; i < nbuiltins; i++) {
        if (strcmp(cmdname, builtin[i].name) == 0)
            return 1;
    }

    return 0;
}

// get_builtin_func: función helper para encontrar función correspondiente
static builtin_func get_builtin_func(char *name) {
    if (name == NULL)
        return NULL;

    for (int i = 0; i < nbuiltins; i++) {
        if (strcmp(name, builtin[i].name) == 0) {
            return builtin[i].func;
        }
    }

    return NULL;
}

// execute_builtin: ejecutar comando builtin, llamando a función correspondiente
int execute_builtin(struct execcmd *ecmd) {
    builtin_func func = get_builtin_func(ecmd->argv[0]);
    if (func)
        return func(ecmd);
    return -1;
}

// get_argc: get arguments count
static int get_argc(char **argv) {
    int cnt = 0;
    while (argv[cnt] != NULL)
        cnt++;
    return cnt;
}

int builtin_cd(struct execcmd *ecmd) {
    char **argv = ecmd->argv;
    int argc = get_argc(argv);

    if (argc > 2)
        panic("cd: too many argumments");

    if (argc == 1) {
        char *homepath = getenv("HOME");
        if (chdir(homepath) < 0) {
            perror("cd");
            return -1;
        }
    } else if (argc == 2) {
        if (chdir(argv[1]) < 0) {
            fprintf(stderr, "cd: %s:", argv[1]);
            perror(NULL);
            return -1;
        }
    }

    return 0;
}

int builtin_exit(struct execcmd *ecmd) {
    exit(EXIT_SUCCESS);
    return 0;
}

int builtin_pwd(struct execcmd *ecmd) {
    char cwd[1024];

    if ((getcwd(cwd, sizeof(cwd))) == NULL) {
        perror("cwd");
        return -1;
    }

    printf("%s\n", cwd);
    return 0;
}

int builtin_help(struct execcmd *ecmd) {
    printf("mish built-in commands:\n");
    printf("  cd [dir]     - Change directory\n");
    printf("  pwd          - Print working directory\n");
    printf("  exit         - Exit the shell\n");
    printf("  help         - Show this help\n");
    printf("  history      - Show command history\n");
    return 0;
}

int builtin_history(struct execcmd *ecmd) {
    char **argv = ecmd->argv;
    int argc = get_argc(argv);

    if (argc > 2)
        panic("history: too many arguments");

    int last = -1;
    if (argc == 2) {
        if (argv[1][0] == '-') {
            switch (argv[1][1]) {
            case 'c':
                clear_history();
                break;
            default:
                panic("history: invalid option");
                break;
            }
            return 0;
        } else {
            char *p;
            last = strtol(argv[1], &p, 10);
            if (*p != '\0')
                panic("history: numeric argument required");
        }
    }

    HIST_ENTRY **list = history_list();
    int len = history_get_history_state()->length;

    int i = ((last != -1) ? ((len - last < 0)? 0 : len - last): 0);

    for (; i < len; i++) {
        printf("%5d %s\n", i + 1, list[i]->line);
    }

    return 0;
}
