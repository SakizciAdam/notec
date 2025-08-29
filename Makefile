CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

TARGET = build/notec

SRCS = main.c control.c writing.c utils.c
OBJS = $(patsubst %.c, build/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean
