#pragma once

#define MAXCWD 1024
#define MAXPROMPT MAXCWD+128

char *get_cwd();
char *get_prompt(char *name);
