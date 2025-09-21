CC = gcc
CFLAGS = -Wall -std=gnu99 -pedantic -g

TARGET = build/mish

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJS = $(SRCS:src/%.c=build/obj/%.o)


all: directories $(TARGET)

directories:
	@mkdir -p build/obj

$(TARGET): $(OBJS)
	$(CC) -o $@ $^
	@echo "build complete: $@"

build/obj/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -rf build/
