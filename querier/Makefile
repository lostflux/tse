# Makefile for TSE indexer
#
# Amittai J. Wekesa, May 2021

# Flag to print indexer progress
TESTFLAGS=-DQUICKTEST

FUZZTEST = fuzztest

# Testing Programs
# INDEXTEST=indextest
VALGRIND= valgrind --leak-check=full --show-leak-kinds=all

# Standard flags 
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -g -ggdb -I ../libcs50 -I ../common

# Program name
PROG = querier

# Objects
OBJS = querier.o query.o

# Libraries
LLIBS = ../common/common.a ../libcs50/libcs50.a 


# indexer functionality (without tests, et al.)
all: 
	make -C ../common
	make $(PROG)
	make $(FUZZTEST)
	
# program  (querier)
$(PROG): $(OBJS) $(LLIBS) 	
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o $@
	# rm -f $(OBJS)


# indexer object file
querier.o: querier.c query.o $(LLIBS)

query.o: query.h

fuzztest.o: fuzzquery.c


# rebuild common library
../common/common.a:
	make clean -C ../common
	make -C ../common


# .PHONY make target
.PHONY: clean test memcheck clean

test:
	make all
	bash -v ./testing.sh

memcheck:
	make all:
	bash -v ./memcheck.sh

# quick test
quicktest: querier.c query.o $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o querier
	./fuzztest ../data/output/toscrape-1.index 100 2021 | ./querier

#fuzztest
fuzztest: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o fuzztest
	make $(PROG)


# clean-up target
clean:
	rm -f core *core.*
	rm -f $(PROG) *~ *.o
	rm -f $(FUZZTEST) *~ *.o


# check ./indexer for memory usage and leaks
# usually takes a while to complete.
memcheck:
	make all
	bash -v ./memcheck.sh
