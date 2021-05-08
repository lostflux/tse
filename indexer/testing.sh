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

To generate these directories and their contents, run the crawler test script "testing.sh".

In turn, this script creats *.index files in the same directory ../data/output.
'

# build source directories:
# make all -C ../crawler
# bash -v ../crawler/testing.sh


# incorrect usage

# no args 
./indexer

# one arg (too few)
./indexer ../data/output/wikipedia-1 

# three args (too many)
./indexer ../data/output/wikipedia-1 ../data/output/wikipedia-1.index ../data/output/wikipedia-1.index

# invalid source directory
./indexer ../data/output/wikipedia-1000 ../data/output/wikipedia-1.index

# invalid output directory (wherein to output create file)
./indexer ../data/output/wikipedia-1 ../data/outputDUMMY/wikipedia-1.index


# VALID TESTS:

# letters, maxDepth = 0
./indexer ../data/output/letters-0 ../data/output/letters-0.index

# letters, maxDepth = 10
./indexer ../data/output/letters-10 ../data/output/letters-10.index

# toscrape, maxDepth = 0
./indexer ../data/output/toscrape-0 ../data/output/toscrape-0.index

# toscrape, maxDepth = 1
./indexer ../data/output/toscrape-1 ../data/output/toscrape-1.index

# wikipedia, maxDepth = 0
./indexer ../data/output/wikipedia-0 ../data/output/wikipedia-0.index

# wikipedia, maxDepth = 1
./indexer ../data/output/wikipedia-1 ../data/output/wikipedia-1.index

