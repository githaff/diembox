
CC=gcc
CFLAGS=

tobin : main.c
	$(CC) $(CFLAGS) -o tobin main.c

clean :
	rm -f tobin


.PHONY : clean
