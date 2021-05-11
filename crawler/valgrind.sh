#!/bin/bash
#
# valgrind.sh
# 
# bash script to run valgrind tests on the crawler
# usage:
#   ./testing.sh
# output:
#   stdout
#
# Amittai Wekesa, April 2021


# build directories
if ! [[ -r ../data/output/letters-0/.crawler ]] ; then
  mkdir -p ../data/test/{letters-0,letters-10,toscrape-0,toscrape-1,wikipedia-0,wikipedia-1}
fi;

# for memory-leak tests
VALGRIND=(valgrind --leak-check=full --show-leak-kinds=all)

printf "\n\n\n"
# letters, maxDepth = 0
"${VALGRIND[@]}" ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/output/letters-0 0

printf "\n\n\n"
# letters, maxDepth = 10
"${VALGRIND[@]}" ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/output/letters-10 10

printf "\n\n\n"
# toscrape, maxDepth = 0
"${VALGRIND[@]}" ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/output/toscrape-0 0

printf "\n\n\n"
# toscrape, maxDepth = 1
"${VALGRIND[@]}" ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/output/toscrape-1 1

printf "\n\n\n"
# wikipedia, maxDepth = 0
"${VALGRIND[@]}" ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/output/wikipedia-0 0

printf "\n\n\n"
# wikipedia, maxDepth = 1
"${VALGRIND[@]}" ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/output/wikipedia-1 1