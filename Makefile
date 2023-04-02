CC=gcc
CFLAGS=\
	-Werror=uninitialized \
	-Werror=return-type \
	-Werror=implicit-function-declaration \
	-Wall -Wextra -pedantic -std=c99 -g \
	-DDEBUG \
	-DPLATFORM=PLATFORM_LINUX \
	-DENABLE_FLOAT \
	-DENABLE_RANDOM \
	-DENABLE_CHAR \
	-DENABLE_FILE \
	-lSDL2

LDFLAGS=-lm
MAIN=src/main.c

TARGET=nanoc

all: $(MAIN)
	$(CC) -o $(TARGET) $(MAIN) $(CFLAGS) $(LDFLAGS)

.PHONY: avr
avr:
	make -f avr.mk
	avr-nm --size-sort -t d -S main.elf

.PHONY: check
check:
	avrdude -pm328p -carduino -P/dev/ttyUSB2 -b57600

.PHONY: upload
upload:
	avrdude -pm328p -carduino -P/dev/ttyUSB2 -b57600 -U main.elf

.PHONY: clean
clean:
	rm -f $(TARGET)
