# Directorio para testear comando miprof

## Ejemplos

```sh
# Ejemplo básico
miprof ejec ls -la

# Ejemplo con guardando en archivo
miprof ejecsave profile.txt ls -la
miprof ejecsave profile.txt ps aux # se añade a profile.txt

# Ejemplo con tiempo limite
miprof ejecutar 5 sleep 10

# Comando directo (tratado como modo ejec)
miprof ps aux
```

## Experimento `sort`

```sh
# Crear archivos para sort
seq 1000 > test1.txt
seq 10000 > test2.txt
seq 100000 > test3.txt

# Testear sort con diferentes archivos
miprof ejecsave sort.txt sort test1.txt
miprof ejecsave sort.txt sort test2.txt
miprof ejecsave sort.txt sort test3.txt
```
