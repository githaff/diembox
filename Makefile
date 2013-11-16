
CC=gcc
CFLAGS=-std=c99 -Werror -Wpedantic -Wall -Wextra

INCLUDES = $(wildcard *.h)

SRC = main.o \
      usage.o
SRC_TOOLS = tobin.c

OBJ_TOOLS = $(SRC_TOOLS:.c=.o)
OBJ = $(SRC:.c=.o) $(OBJ_TOOLS)
LINKS = $(OBJ_TOOLS:.o=)


all : embox $(LINKS)

%.o : %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $<

embox : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(LINKS) :
	ln -s embox $@

clean :
	rm -f embox
	rm -f *.o
	rm -f $(LINKS)
	rm -f *~ 

.PHONY : clean
