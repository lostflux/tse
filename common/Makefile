# Makefile for common directory
#
# Amittai J. Wekesa, April 2021

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I ../libcs50
OBJS = pagedir.o index.o word.o ../libcs50/hashtable.o ../libcs50/counters.o ../libcs50/set.o
LIB = common.a
LLIBS = ../libcs50/libcs50-given.a
MAKE = make


all: 
	make $(LIB)
# Rules
$(LIB): $(OBJS) $(LLIBS)
	ar cr $(LIB) $^

pagedir.o: pagedir.h

index.o: index.h

word.o: word.h

../libcs50/hashtable.o: ../libcs50/hashtable.h

../libcs50/counters.o: ../libcs50/counters.h

../libcs50/set.o: ../libcs50/set.h

# Phony Targets
.PHONY: clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o