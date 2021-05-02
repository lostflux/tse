# CS50 TSE

## Amittai J. Wekesa (@siavava)

### Basic idea

This directory (./crawler) implements functionality for a Search Engine crawler.

This crawler expects the following commandline arguments:

1. A url to search from.

2. An existing directory wherein to save data about visited webpages.

3. A maximum depth of search (must be >= 0).

### Functionality

The crawler runs a [Depth-First Search](https://en.wikipedia.org/wiki/Depth-first_search) from the seed URL, identifying and saving webpages that fit a pre-specified criterion. The idea is to later be able to index the saved directories, query, and find webpages.

To build, run `make`.

To clean up, run `make clean`.

To test the crawler module, run `make test`. To see previous test results, check out **testing.out** generated from **testing.sh**.

To run memory tests, run `make valgrind`. To see previous memory results, check out **valgrind.out** generated from **valgrind.sh**.
