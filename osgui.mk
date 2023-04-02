all:
	gcc main.c lib/lib.c -Ilib/ -o osgui -Wall -Wextra -O2 -std=c89 -lSDL2

clean:
	rm -f osgui
