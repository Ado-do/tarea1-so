#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Lista de comandos internos */
char *builtin_str[] = {
    "cd",
    "exit"};

int num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/* Implementaciones de los comandos internos */
int mi_shell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "mi_shell: se esperaba un argumento para \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("mi_shell");
        }
    }
    return 1;
}

int mi_shell_exit(char **args)
{
    exit(EXIT_SUCCESS);
}

int (*builtin_func[])(char **) = {
    &mi_shell_cd,
    &mi_shell_exit};

char *read_line()
{
    char *line = NULL;
    size_t bufsize = 0; // Usado por getline
    getline(&line, &bufsize, stdin);
    return line;
}

char **split_line(char *line)
{
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "mi_shell: error de asignacion\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\r\n\a");
    while (token != NULL)
    {
        printf("Token extraido: %s\n", token);
        tokens[position] = token;
        position++;
        // Reasignar memoria si es necesario
        if (position >= bufsize)
        {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "mi_shell: error de reasignacion\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}
int command_execute(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // Estamos en el proceso hijo
        // Se llama a execvp para ejecutar el comando
        if (execvp(args[0], args) == -1)
        {
            perror("mi_shell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // Error en la creación del fork
        perror("mi_shell");
    }
    else
    {
        // Estamos en el proceso padre
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
int execute(char **args)
{
    if (args[0] == NULL)
    {
        return 1;
    }
    // Buscamos si la linea ingresada tiene comandos internos como CD
    for (int i = 0; i < num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }
    return command_execute(args);
}
void shell_loop()
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("mi_shell> "); // promt
        line = read_line();   // Leer la línea del usuario
        args = split_line(line);
        execute(args);
        free(args);
        free(line);
    } while (1);
}

int main(int argc, char **argv)
{
    shell_loop();
    return EXIT_SUCCESS;
}