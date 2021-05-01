# Makefile for common directory
#
# Amittai J. Wekesa, April 2021

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50
OBJS = pagedir.o
LIB = common.a
LLIBS = ../libcs50/libcs50-given.a
MAKE = make

# Rules
$(LIB): $(OBJS) $(LLIBS)
	ar cr $(LIB) $^

pagedir.o: pagedir.h
index.o: index.h
word.o: word.h

# Phony Targets
.PHONY: clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o