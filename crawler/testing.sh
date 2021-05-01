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

# one arg (too few)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/

# two args (too few)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-6

# four args (too many)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-6 6 dummy-arg

# three args (maxDepth invalid)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-10 -1

# three args (valid)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/output/letters-10 10

# three args (valid)
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/output/wikipedia-3 3