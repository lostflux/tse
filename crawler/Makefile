# Makefile for crawler.c
#
# Amittai J. Wekesa, April 2021

# Flag to print crawler progress
TESTFLAGS=-DAPPTEST

# Standard flags 
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 -I ../common

# Program name
PROG = crawler

# Objects
OBJS = crawler.o

# Libraries
LLIBS = ../libcs50/libcs50-given.a ../common/common.a

all: $(PROG) $(LLIBS)

$(PROG): $(OBJS) $(LLIBS) 	
	$(CC) $(CFLAGS) $^ -o $@

crawler.o: crawler.c -DAPPTEST $(LLIBS)

../common/common.a:
	make clean -C ../common
	make -C ../common

.PHONY: clean test valgrind clean

test: crawler.c $(LLIBS) 
	$(CC) $(CFLAGS) -DAPPTEST $^ -o crawler
	bash -v ./testing.sh

clean:
	rm -f core *core.*
	rm -f $(PROG) *~ *.o

valgrind: crawler.c $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o crawler
	
	bash -v valgrind.sh
