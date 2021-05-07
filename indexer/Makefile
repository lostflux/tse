# Makefile for TSE indexer
#
# Amittai J. Wekesa, May 2021

# Flag to print crawler progress
TESTFLAGS=-DLOGPROGRESS -DQUICKTEST

# Standard flags 
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 -I ../common

# Program name
PROG = indexer

# Objects
OBJS = indexer.o

# Libraries
LLIBS = ../common/common.a ../libcs50/libcs50-given.a 

all: 
	make -C ../common
	make $(PROG)
	

$(PROG): $(OBJS) $(LLIBS) 	
	$(CC) $(CFLAGS) $^ -o $@
	rm -f $(OBJS)

indexer.o: indexer.c $(LLIBS)

../common/common.a:
	make clean -C ../common
	make -C ../common

.PHONY: clean test valgrind clean

test: indexer.c $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o indexer
	./indexer

clean:
	rm -f core *core.*
	rm -f $(PROG) *~ *.o

valgrind: indexer.c $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o indexer
	valgrind --leak-check=full --show-leak-kinds=all ./indexer
