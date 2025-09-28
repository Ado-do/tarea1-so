#include "miprof.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// get_timestamp: obtener tiempo actual como string
char *get_timestamp() {
    static char buffer[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

// write_profiling_data: escribir resultados profiling en archivo
void write_profiling_data(struct prof_data *data, const char *filename, const char *command) {
    int fd;
    if (filename) {
        int file_permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, file_permissions);
        if (fd < 0) {
            perror("miprof: cannot open file");
            return;
        }
    } else {
        fd = STDOUT_FILENO;
    }

    char buffer[1024];
    double real_time = (data->end_time.tv_sec - data->start_time.tv_sec) +
                       (data->end_time.tv_usec - data->start_time.tv_usec) / 1000000.0;
    double user_time = data->usage.ru_utime.tv_sec + data->usage.ru_utime.tv_usec / 1000000.0;
    double sys_time = data->usage.ru_stime.tv_sec + data->usage.ru_stime.tv_usec / 1000000.0;

    int len = snprintf(buffer, sizeof(buffer),
                       "=== miprof Profiling Results ===\n"
                       "Timestamp: %s\n"
                       "Command: %s\n"
                       "Real time: %.3f seconds\n"
                       "User time: %.3f seconds\n"
                       "System time: %.3f seconds\n"
                       "Max RSS: %ld KB\n"
                       "Exit status: %d\n"
                       "===============================\n\n",
                       get_timestamp(), command, real_time, user_time, sys_time, data->max_rss, data->exit_status);

    write(fd, buffer, len);

    if (filename) {
        close(fd);
        printf("miprof: results saved to %s\n", filename);
    }
}

// execute_with_profiling: ejecutar comando con profiling
int execute_with_profiling(char **cmd_argv, const char *output_file, int timeout_seconds) {
    struct prof_data data;
    pid_t pid;
    int status;

    // Guardar string que contiene todo el comando
    char full_cmd[1024] = {0};
    for (int i = 0; cmd_argv[i] != NULL; i++) {
        if (i > 0) {
            strcat(full_cmd, " ");
        }
        strncat(full_cmd, cmd_argv[i], sizeof(full_cmd) - strlen(full_cmd) - 1);
    }

    gettimeofday(&data.start_time, NULL);

    pid = fork();
    if (pid < 0) {
        perror("miprof: fork failed");
        return -1;
    }

    if (pid == 0) {
        // Proceso hijo: ejecutar comando
        execvp(cmd_argv[0], cmd_argv);
        perror("miprof: exec failed");
        exit(EXIT_FAILURE);
    } else {
        // Proceso padre: monitorear ejecución
        pid_t timeout_pid = 0;
        int timeout_occurred = 0;

        // Configurar timeout si fue especificado
        if (timeout_seconds > 0) {
            timeout_pid = fork();
            if (timeout_pid < 0) {
                perror("miprof: timeout fork failed");
                // Continue without timeout
            } else if (timeout_pid == 0) {
                sleep(timeout_seconds);
                exit(EXIT_SUCCESS);
            }
        }

        // Esperar por el término del comando o timeout
        pid_t finished_pid;
        if (timeout_pid > 0) {
            // Esperar por cualquiera de los dos procesos
            finished_pid = wait(&status);

            if (finished_pid == timeout_pid) {
                // Timeout ocurrió primero
                printf("miprof: command timed out after %d seconds\n", timeout_seconds);
                kill(pid, SIGKILL);
                waitpid(pid, &status, 0);
                timeout_occurred = 1;
            } else if (finished_pid == pid) {
                // Comando terminó antes del timeout
                kill(timeout_pid, SIGKILL);
                waitpid(timeout_pid, NULL, 0);
            }
        } else {
            // Sin timeout, solo esperar el comando
            finished_pid = waitpid(pid, &status, 0);
        }

        gettimeofday(&data.end_time, NULL);

        // Obtener estadísticas de uso de recursos
        if (getrusage(RUSAGE_CHILDREN, &data.usage) < 0) {
            perror("miprof: getrusage failed");
            memset(&data.usage, 0, sizeof(data.usage));
        }
        data.max_rss = data.usage.ru_maxrss;

        if (timeout_occurred) {
            data.exit_status = -1;
        } else if (WIFEXITED(status)) {
            data.exit_status = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            data.exit_status = 128 + WTERMSIG(status); // Standard Unix convention
        } else {
            data.exit_status = -1;
        }

        // Mostrar o guardar resultados
        write_profiling_data(&data, output_file, full_cmd);

        return data.exit_status;
    }
}

int miprof(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "miprof: usage: miprof [ejec|ejecsave file|ejecutar timeout] command [args]\n");
        return -1;
    }

    char *output_file = NULL;
    int timeout_seconds = 0;
    int command_start_index = 1;

    // Parsear argumentos del comando
    if (strcmp(argv[1], "ejec") == 0) {
        // Solo ejecutar y mostrar
        output_file = NULL;
        command_start_index = 2;
    } else if (strcmp(argv[1], "ejecsave") == 0) {
        // Ejecutar y guardar en archivo
        if (argc < 3) {
            fprintf(stderr, "miprof: ejecsave requires filename\n");
            return -1;
        }
        output_file = argv[2];
        command_start_index = 3;
    } else if (strcmp(argv[1], "ejecutar") == 0) {
        // Ejecutar con tiempo límite
        if (argc < 4) {
            fprintf(stderr, "miprof: ejecutar requires timeout and command\n");
            return -1;
        }
        timeout_seconds = atoi(argv[2]);
        if (timeout_seconds <= 0) {
            fprintf(stderr, "miprof: invalid timeout value: %s\n", argv[2]);
            return -1;
        }
        command_start_index = 3;
    }

    // Verificar si hay un comando que ejecutar
    if (command_start_index >= argc) {
        fprintf(stderr, "miprof: no command specified\n");
        return -1;
    }

    char **command_argv = &argv[command_start_index];

    int result = execute_with_profiling(command_argv, output_file, timeout_seconds);
    return result;
}
