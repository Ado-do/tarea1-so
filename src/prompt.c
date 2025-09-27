#include "prompt.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define G "\033[1;32m" // Aplicar verde
#define RST "\033[0m"  // Resetear a blanco

char *get_cwd() {
    static char cwd[MAXCWD];
    return ((getcwd(cwd, MAXCWD) != NULL)? cwd : "?");
}

char *get_prompt(char *name) {
    static char prompt[MAXPROMPT];

    char *cwd = strrchr(get_cwd(), '/') + 1;

    snprintf(prompt, MAXPROMPT, G "[%s: " RST "%s"G"]> " RST, name, cwd);
    return prompt;
}
