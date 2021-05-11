# Makefile for common directory
#
# Amittai J. Wekesa, April 2021

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50
OBJS = pagedir.o index.o word.o
LIB = common.a
LLIBS = ../libcs50/libcs50.a
MAKE = make

all: 
	# ar -x $(LLIBS)
	make $(LIB)
	rm -f *.o
# Rules
$(LIB): $(OBJS)
	ar cr $(LIB) $^

pagedir.o: pagedir.h
index.o: index.h
word.o: word.h


# Phony Targets
.PHONY: clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o