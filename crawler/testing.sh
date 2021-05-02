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
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/output/letters-6 6 dummy-arg

# three args (maxDepth invalid)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/output/letters-10 -1


# VALID TESTS:

# letters, maxDepth = 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/output/letters-0 0

# letters, maxDepth = 10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/output/letters-10 10

# toscrape, maxDepth = 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/output/toscrape-0 0

# toscrape, maxDepth = 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/output/toscrape-1 1

# wikipedia, maxDepth = 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/output/wikipedia-0 0

# wikipedia, maxDepth = 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/output/wikipedia-1 1