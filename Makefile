
CC=gcc
CFLAGS=-Werror -Wpedantic -Wall -Wextra

TOOL_OBJS = tobin.o
OBJ = main.o $(TOOL_OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

ltool : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm -f ltools
	rm -f *.o
	rm -f *~ 

.PHONY : clean
