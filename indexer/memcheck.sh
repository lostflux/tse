#!/bin/bash
#
# memcheck.sh
# usage:
#   ./memcheck.sh
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

To generate these directories and their contents, run the crawler test script "testing.sh".

In turn, this script creats *.index files in the same directory ../data/output.
'

VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"

# incorrect usage

# no args 
$VALGRIND ./indexer

# one arg (too few)
$VALGRIND ./indexer ../data/output/wikipedia-1 

# three args (too many)
$VALGRIND ./indexer ../data/output/wikipedia-1 ../data/output/wikipedia-1.index ../data/output/wikipedia-1.index

# invalid source directory
$VALGRIND ./indexer ../data/output/wikipedia-1000 ../data/output/wikipedia-1.index

# invalid output directory (wherein to output create file)
$VALGRIND ./indexer ../data/output/wikipedia-1 ../data/outputDUMMY/wikipedia-1.index


# VALID TESTS:

# letters, maxDepth = 0
$VALGRIND ./indexer ../data/output/letters-0 ../data/output/letters-0.index

# letters, maxDepth = 10
$VALGRIND ./indexer ../data/output/letters-10 ../data/output/letters-10.index

# toscrape, maxDepth = 0
$VALGRIND ./indexer ../data/output/toscrape-0 ../data/output/toscrape-0.index

# toscrape, maxDepth = 1
$VALGRIND ./indexer ../data/output/toscrape-1 ../data/output/toscrape-1.index

# wikipedia, maxDepth = 0
$VALGRIND ./indexer ../data/output/wikipedia-0 ../data/output/wikipedia-0.index

# wikipedia, maxDepth = 1
$VALGRIND ./indexer ../data/output/wikipedia-1 ../data/output/wikipedia-1.index

