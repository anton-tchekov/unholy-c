CC=gcc
CFLAGS=\
	-Werror=uninitialized \
	-Werror=return-type \
	-Werror=implicit-function-declaration \
	-Wall -Wextra -pedantic -std=c99 -O2 -g \
	-DDEBUG \
	-DPLATFORM=PLATFORM_LINUX \
	-DENABLE_FLOAT \
	-DENABLE_RANDOM \
	-DENABLE_CHAR \
	-DENABLE_FILE

LDFLAGS=-lm
MAIN=src/main.c
SRC=src/builtin.c \
	src/error.c \
	src/instr.c \
	src/interpreter.c \
	src/memory.c \
	src/parser.c \
	src/stream.c \
	src/token.c \
	src/tokenizer.c \
	src/util.c \
	$(MAIN)

TARGET=nanoc

all: $(SRC)
	$(CC) -o $(TARGET) $(MAIN) $(CFLAGS) $(LDFLAGS)

.PHONY: avr
avr:
	make -f avr.mk
	avr-nm --size-sort -t d -S main.elf

.PHONY: clean
clean:
	rm -f $(TARGET)
