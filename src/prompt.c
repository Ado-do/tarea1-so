#include "prompt.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *get_current_dir() {
    static char cwd[MAXCWD];
    return ((getcwd(cwd, MAXCWD) != NULL)? cwd : "?");
}

char *get_prompt(char *name) {
    static char prompt[MAXPROMPT];

    char *home = getenv("HOME");
    char *cwd = get_current_dir();
    if (strcmp(cwd, home)) {
        cwd += strlen(home) - 1;
        *cwd = '~';
    }

    snprintf(prompt, MAXPROMPT, "%s: %s > ", name, cwd);
    return prompt;
}
