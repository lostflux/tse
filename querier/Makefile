# Makefile for TSE indexer
#
# Amittai J. Wekesa, May 2021

# Flag to print indexer progress
TESTFLAGS=-DQUICKTEST

# Testing Programs
# INDEXTEST=indextest
VALGRIND= valgrind --leak-check=full --show-leak-kinds=all

# Standard flags 
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50 -I ../common

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
# make querytest
	
# program  (querier)
$(PROG): $(OBJS) $(LLIBS) 	
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o $@
	# rm -f $(OBJS)


# indexer object file
querier.o: querier.c query.o $(LLIBS)

query.o: query.h


# rebuild common library
../common/common.a:
	make clean -C ../common
	make -C ../common


# .PHONY make target
.PHONY: clean test memcheck clean


# test run (rebuilds crawler and generates files)
test: querier.c query.o $(LLIBS) 
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o querier
	# ./querier
	# printf "\nBuilding crawler directories... ... ...\n\n"
	# make test -C ../crawler
	# bash -v ./testing.sh


# indextest -- unit test for index data-structure
indextest: indextest.c $(LLIBS)
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o indextest
	bash -v ./indextest.sh


# clean-up target
clean:
	rm -f core *core.*
	rm -f $(PROG) *~ *.o
	# rm -f $(INDEXTEST) *~ *.o


# check ./indexer for memory usage and leaks
# usually takes a while to complete.
memcheck: indexer.c $(LLIBS)
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o indexer
	bash -v ./memcheck.sh


# check ./indextest for memory usage and leaks.
# usually takes a while to complete.
indextest_memcheck: indextest.c $(LLIBS)
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o indextest
	bash -v ./indextest_memcheck.sh