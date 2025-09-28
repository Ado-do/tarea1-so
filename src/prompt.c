#include "prompt.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define GREEN "\033[1;32m" // Aplicar verde
#define RESET "\033[0m"  // Resetear a blanco

char *get_cwd() {
    static char cwd[MAXCWD];
    return ((getcwd(cwd, MAXCWD) != NULL)? cwd : "?");
}

char *get_prompt() {
    static char prompt[MAXPROMPT];
    char *cwd = strrchr(get_cwd(), '/') + 1;
    snprintf(prompt, MAXPROMPT, GREEN"[mish: "RESET"%s"GREEN"]> "RESET, cwd);
    return prompt;
}
