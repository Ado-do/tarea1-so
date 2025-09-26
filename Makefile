CC = gcc
CFLAGS = -Wall -std=gnu99 -Og
LD_FLAGS = -lreadline

TARGET = build/mish

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJS = $(SRCS:src/%.c=build/obj/%.o)


.PHONY: all run clean dirs

all: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LD_FLAGS)
	@echo "build shell complete: $@"
	@echo "run shell: make run"

build/obj/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

dirs:
	@mkdir -p build build/obj

run: all
	./$(TARGET)

clean:
	rm -rf build/

