#!/bin/bash
#
# indextest_memcheck.sh
# usage:
#   ./indextest_memcheck.sh
# output:
#   stdout
#
# Author: Amittai Wekesa, May 2021

: '
DISCLAIMER: The functionality of this testing script is dependent on
the existence of the following directories in the directory ../data/output 
1. letters-0
2. letters-10
3. toscrape-0
4. toscrape-1
5. wikipedia-0
6. wikipedia-1

To generate these directories and their contents, run `make test`, 
which tests the indexer and builds the needed files.

This script reads created "[...].index" files from ..data/output, 
rebuilds them into index_t objects using the indextest module defined in indextest.c,
and writes them back into "...-rebuild.index" files in ../data/output.

It then runs the comparator function in ~/cs50-dev/shared/tse/indexcmp,
which compares the original and rebuild files and checks for discrepancies.
'

VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"
COMPARATOR=~/cs50-dev/shared/tse/indexcmp

# incorrect usage

# no args 
$VALGRIND ./indextest

# one arg (too few)
$VALGRIND ./indextest ../data/output/wikipedia-1.index

# three args (too many)
$VALGRIND ./indextest ../data/output/wikipedia-1.index ../data/output/wikipedia-1.index ../data/output/wikipedia-1.index

# invalid source directory
$VALGRIND ./indextest ../data/output/wikipedia-1000.index ../data/output/wikipedia-1000-rebuild.index

# invalid output directory (wherein to output create file)
$VALGRIND ./indextest ../data/output/wikipedia-1.index ../data/outputDUMMY/wikipedia-1-rebuild.index


# VALID TESTS:

# letters, maxDepth = 0
$VALGRIND ./indextest ../data/output/letters-0.index ../data/output/letters-0-rebuild.index

# letters, maxDepth = 10
$VALGRIND ./indextest ../data/output/letters-10.index ../data/output/letters-10-rebuild.index

# toscrape, maxDepth = 0
$VALGRIND ./indextest ../data/output/toscrape-0.index ../data/output/toscrape-0-rebuild.index

# toscrape, maxDepth = 1
$VALGRIND ./indextest ../data/output/toscrape-1.index ../data/output/toscrape-1-rebuild.index

# wikipedia, maxDepth = 0
$VALGRIND ./indextest ../data/output/wikipedia-0.index ../data/output/wikipedia-0-rebuild.index

# wikipedia, maxDepth = 1
$VALGRIND ./indextest ../data/output/wikipedia-1.index ../data/output/wikipedia-1-rebuild.index
