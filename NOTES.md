# NOTAS

## 1. Implementación básica shell

- [x] 1.1. Prompt
  - `write(int fd, const void *buf, size_t count)`: Para escribir el prompt en la salida estándar (fd = 1).

- [x] 1.2. Lectura de comando
  - `read(int fd, void *buf, size_t count)`: Para leer la entrada del teclado (fd = 0).
  - Aparte, GNU readline usa: `read`, `write`, `ioctl`, `open`, `select`

- [x] 1.3. Comando exit
  - `exit(int status)`: Para finalizar el proceso de la shell.

- [x] 1.4. Manejar caso en que comando no existe
  - `write(int fd, const void *buf, size_t count)`: Para imprimir el mensaje de error en la salida estándar.

- [x] 1.5. Manejar caso de linea vacía terminada en newline
  - `write(int fd, const void *buf, size_t count)`: Para volver a imprimir el prompt.

- [x] 1.6. Ejecutar comando de manera concurrente (fork() + exec())

- [x] 1.7. Soporte de pipes


## 2. Implementación comando personalizado *miprof*

- [x] 2.1 `miprof [ejec|ejecsave archivo] comando args`

- [ ] 2.2 `miprof` + `sort`, interpretar.
  - `sort` demorará más según la cantidad de elementos que deba ordenar (O(nlogn))
  - `sort` utiliza más memoria a medida que el input es muy grande

- [x] 2.3 `miprof ejecutar maxtiempo comando args`

