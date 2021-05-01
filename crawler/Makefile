# Makefile for crawler.c
#
# Amittai J. Wekesa, April 2021


CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 -I ../common
PROG = crawler
OBJS = crawler.o
LLIBS = ../libcs50/libcs50-given.a ../common/common.a


all: crawler ../common/common.a

crawler: $(OBJS) $(LLIBS)	
	$(CC) $(CFLAGS) $^ -o $@

# crawler.o: crawler.c $(LIBS)

../common/common.a:
	make clean -C ../common
	make -C ../common

.PHONY: clean test valgrind clean

test:
	make all
	bash -v ./testing.sh

clean:
	rm -f core *core.*
	rm -f $(PROG) *~ *.o

valgrind:
	valgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/ output/letters-depth-6 6

