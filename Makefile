
CC=gcc
CFLAGS=-Werror -Wpedantic -Wall -Wextra

tobin : main.c
	$(CC) $(CFLAGS) -o tobin main.c

clean :
	rm -f tobin *~


.PHONY : clean
