CC=gcc
CFLAGS=\
	-Ilib/ \
	-Wall -Wextra \
	-Werror=uninitialized \
	-Werror=return-type \
	-Werror=implicit-function-declaration \
	-pedantic -std=c99 \
	-g -DDEBUG \
	-lSDL2

LDFLAGS=-lm
MAIN=src/main.c \
	platform/simulator/platform.c

TARGET=platform/simulator/main

.PHONY: all
all:
	$(CC) -o $(TARGET) $(MAIN) $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)
