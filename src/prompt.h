#pragma once

#define MAXCWD 1024
#define MAXPROMPT MAXCWD+128

char *get_current_dir();
char *get_prompt(char *name);
