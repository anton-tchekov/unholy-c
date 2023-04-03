.PHONY: simulator
simulator:
	make -f platform/simulator/Makefile

.PHONY: run
run:
	./platform/simulator/main $(FILE)

.PHONY: avr
avr:
	make -f platform/avr/Makefile
	avr-nm --size-sort -t d -S main.elf

.PHONY: check
check:
	avrdude -pm328p -carduino -P/dev/ttyUSB0 -b57600

.PHONY: upload
upload:
	avrdude -pm328p -carduino -P/dev/ttyUSB0 -b57600 -U main.elf
