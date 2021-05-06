# Makefile for TSE indexer
#
# Amittai J. Wekesa, May 2021

# Flag to print crawler progress
TESTFLAGS=-DAPPTEST

# Standard flags 
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 -I ../common

# Program name
PROG = indexer

# Objects
OBJS = indexer.o

# Libraries
LLIBS = ../libcs50/libcs50-given.a ../common/common.a

all: $(PROG) $(LLIBS)
	
	

$(PROG): $(OBJS) $(LLIBS) 	
	$(CC) $(CFLAGS) $^ -o $@

indexer.o: indexer.c $(LLIBS)

../common/common.a:
	make clean -C ../common
	make -C ../common

.PHONY: clean test valgrind clean

test: indexer.c $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o indexer
	bash -v ./testing.sh

clean:
	rm -f core *core.*
	rm -f $(PROG) *~ *.o

valgrind: indexer.c $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o crawler
	
	bash -v valgrind.sh
