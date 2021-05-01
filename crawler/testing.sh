#!/bin/bash
#
# testing.sh
# usage:
#   ./testing.sh
# output:
#   stdout
#
# Amittai Wekesa, April 2021


# invalid usage

# no args 
./crawler

# one arg
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/

# two args
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-6

# four args
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-6 6 dummy-arg

# three args (valid)
valgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-10 10

