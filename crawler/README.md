# CS50 TSE

## Amittai J. Wekesa (@siavava)

### Basic idea

This directory (./crawler) implements functionality for a Search Engine crawler.

This crawler expects the following commandline arguments:

1. A url to search from.

2. An existing directory wherein to save data about visited webpages.

3. A maximum depth of search (must be >= 0).

***

### Functionality

The crawler runs a [Depth-First Search](https://en.wikipedia.org/wiki/Depth-first_search) from the seed URL, identifying and saving webpages that fit a pre-specified criterion. The idea is to later be able to index the saved directories, query, and find webpages.

***

To build, run `make`.

To clean up, run `make clean`.

To test the crawler module, run `make test`. Output from previous tests is available in the *testing.out* file, generated from *testing.sh*.

To test memory usage, run `make valgrind`. Output from previous tests is available in teh *valgrind.out* file, generated from *valgrind.sh*.

Note: the testing scripts (*testing.sh* and *valgrind.sh*) anticipate the existence of a `./data/output/[FOLDER]` location where `[FOLDER]` is a folder named in the fashion `seedURL-maxDepth` (for example, `letters-0`, `letters-10`, `wikipedia-1`, etc.).

**Please make sure those folders exist before using the testing scripts; they will not work elsewhile.**
