#!/bin/bash
#
# testing.sh
# usage:
#   ./testing.sh
# output:
#   stdout
#
# Author: Amittai Wekesa, April 2021

: '
DISCLAIMER: The functionality of this testing script is dependent on
the existence of the following directories in the directory ../data/output 
1.  letters-0
2. letters-10
3. toscrape-0
4. toscrape-1
5. wikipedia-0
6. wikipedia-1

To generate these directories and their contents, run "make test" in the indexer directory.

In turn, this script creats *.index files in the same directory ../data/output.
'

# check if indexer file do not exist, build them
if ! [[ -r ../data/output/letters-0.index ]] ; then
  # if not, rebuild.
  printf "\nBuilding indexer directories... ... ...\n\n"
  make test -C ../indexer
fi;

# valgrind target
VALGRIND="valgrind -v --leak-check=full --show-leak-kinds=all"

# incorrect usage

# no args 
$VALGRIND ./querier

# one arg (too few)
$VALGRIND ./querier ../data/output/wikipedia-1 

# three args (too many)
$VALGRIND ./querier ../data/output/wikipedia-1 ../data/output/wikipedia-1.index ../data/output/wikipedia-1.index

# invalid source directory
$VALGRIND ./querier ../data/output/wikipedia-1000 ../data/output/wikipedia-1.index

# invalid output directory (wherein to output create file)
$VALGRIND ./querier ../data/output/wikipedia-1 ../data/outputDUMMY/wikipedia-1.index


# VALID TESTS:

# letters, maxDepth = 0
./fuzztest ../data/output/letters-0.index 25 2021 | $VALGRIND ./querier ../data/output/letters-0 ../data/output/letters-0.index

# letters, maxDepth = 10
./fuzztest ../data/output/letters-10.index 25 2021 | $VALGRIND ./querier ../data/output/letters-10 ../data/output/letters-10.index

# toscrape, maxDepth = 0
./fuzztest ../data/output/toscrape-0.index 25 2021 | $VALGRIND ./querier ../data/output/toscrape-0 ../data/output/toscrape-0.index

# toscrape, maxDepth = 1
./fuzztest ../data/output/toscrape-1.index 25 2021 | $VALGRIND ./querier ../data/output/toscrape-1 ../data/output/toscrape-1.index

# wikipedia, maxDepth = 0
./fuzztest ../data/output/wikipedia-0.index 25 2021 | $VALGRIND ./querier ../data/output/wikipedia-0 ../data/output/wikipedia-0.index

# wikipedia, maxDepth = 1
./fuzztest ../data/output/wikipedia-1.index 25 2021 | $VALGRIND ./querier ../data/output/wikipedia-1 ../data/output/wikipedia-1.index

