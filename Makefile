
CC=gcc
CFLAGS=-Werror -Wpedantic -Wall -Wextra

ltools : main.c tobin.c
	$(CC) $(CFLAGS) -o ltools main.c tobin.c

clean :
	rm -f ltools *~


.PHONY : clean
