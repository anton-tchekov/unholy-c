CC=gcc
CFLAGS=-I'include/' \
	-Werror=uninitialized \
	-Werror=return-type \
	-Werror=implicit-function-declaration \
	-Wall -Wextra -pedantic -std=c99 -g -DDEBUG -DPLATFORM=PLATFORM_LINUX

LDFLAGS=-lm

INCLUDE=include
DEPS=types
SRC=src
OBJ=obj
OBJS=test

_DEPS=$(patsubst %, $(INCLUDE)/%.h, $(DEPS))
_OBJS=$(patsubst %, $(OBJ)/%.o, $(OBJS))

TARGET=nanoc

$(OBJ)/%.o: $(SRC)/%.c $(_DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(_OBJS)
	$(CC) -o $(TARGET) $(_OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ)/*.o
