CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -g

TARGET = build/coshell

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/obj/%.o)


all: directories $(TARGET)

directories:
	@mkdir -p build/obj

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@
	@echo "build complete: $@"

build/obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -rf build/
