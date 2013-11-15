
CC=gcc
CFLAGS=-Werror -Wpedantic -Wall -Wextra

TOOL_SRCS = tobin.c
TOOL_OBJS = $(TOOL_SRCS:.c=.o)
OBJ = main.o $(TOOL_OBJS)
LINKS = $(TOOL_OBJS:.o=)


all : ltools $(LINKS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

ltools : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(LINKS) :
	ln -s ltools $@

clean :
	rm -f ltools
	rm -f *.o
	rm -f $(LINKS)
	rm -f *~ 

.PHONY : clean
