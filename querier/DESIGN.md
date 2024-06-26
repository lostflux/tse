# CS50 TSE Querier

## DESIGN

Author: Amittai J. Wekesa (github: @siavava)

***

## Design Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and focus on the implementation-independent design decisions.
T
Design considerations:

- User interface
- Inputs and outputs
- Functional decomposition into modules
- Data flow through modules
- Pseudo code (plain English-like language) for logic/algorithmic flow
- Major data structures
- Testing plan

***

## User interface

As described in the [Requirements Spec](REQUIREMENTS.md), the querier's only interface with the user is on the command-line; it must always have two arguments.

``` bash
./querier pageDirectory indexFilename
```

`pageDirectory` is the pathname to a directory created by the [Crawler](../crawler/README.md).
`indexFilename` ia the path to a file created by the [Indexer](../indexer/README.md).

Furthermore, during runtime the Querier reads in queries from the user through a `FILE` such as `stdin` and processes them, giving feedback through stdout. 
The querier conditionally prints out prompts if the source `FILE` stream is a terminal. 

***

## Inputs and outputs

### Input

1. On initialization, the Querier requires two inputs as listed above: a path to a directory generated by the crawler, and a path to an index file produced by the indexer.

2. During runtime, the querier reads search queries from `stdin`, one per line, until EOF.

### Output

1. The Querier prints cleaned-up queries for the user before running them.

2. The Querier prints query results to `stdout`, ranked by scores; this listing includes:

    - the page's score.

    - the document ID wherein the page is stored.

    - the page's URL.

3. The Querier prints to `stderr` whenever an exception encountered.

4. On termination, the Querier returns an exit code dependent on the condition that prompted an exit:

***

## Functional decomposition into modules

The Querier is composed of the following functions:

 1. `main`: which parses arguments, ascertains that the correct number of arguments were received, and initializes other modules.
 2. `parseArgs`: receives allocated memory from main and the argument vector passed in from `stdin` . It checks that provided directories/files are valid and saves them to the passed in memory. In case of an error (such as a non-existent file), it frees the passed in memory before exiting with an appropriate error code. `stdin`.
 3. `getQuery`: receives a pointer to a valid `FILE` (could be `stdin`) and reads in a query sentence. It stores the words in an array and returns a pointer to that array.
 4. `parseQuery`: does the work of interpreting queries passed in from `stdin`.
 5. `runQuery`: receives an array of split-up subqueries from `parseQuery` and processes them as appropriate.
 6. `prompt`: conditionally prompts the user for new query promps if the input stream is a terminal.


And some helper modules that provide data structures:

 1. `query`: handles the work of finding results and tracking them, creating unions, intenrsections, handling non-existent words, printing query results, etc.
 2. `index`: represents the actual index of words from webpages wherein we search for matches.
 3. `pagedir`: handles essential file IO for the QUerier.
 4. `word`: provides functionality for normalizing words in a passed-in query.
 5. `webpage`: provides a manipulateable representation of a webpage.
 6. `mem`: provides guarded memory alloc and free functionalities, including the ability to check that memory allocated successfully.

***

## Pseudo code for logic/algorithmic flow

The querier will run as follows:

```pseudocode
Start
confirm correct number of arguments was received from command line.
call parseArgs to validate and save arguments.
load the index from provided index file.
prompt use for query.
while *some* query received:
  parse the query.
  run the query.
  free the query.
  prompt for next query.
 
on exit of query loop, delete all instance variables and quit.
```
***

## Major data structures

Helper modules provide all the data structures we need:

 1. `query`: handles the work of finding results and tracking them, creating unions, intenrsections, handling non-existent words, printing query results, etc.
 2. `index`: represents the actual index of words from webpages wherein we search for matches.
 3. `webpage`: provides a manipulateable representation of a webpage.

***

## Testing plan

I've written testing scripts for testing the querier and the query module. Due to their coupled nature, it was most effective to test the two as a unit.

Testing is done using the [fuzzquery](./fuzzquery.c) to generate random queries based on words in the relevant index file and pipe them through the querier.

A sampling of tests that are run:

1. Test the querier with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

2. Query a simple index with only a handful of pages. This is handy to quickly pin down erratic behavior. Verify that the query results match expectations.

3. Repeat with deeper indexes with hundreds, maybe thousands of words and more pages. Make sure the querier handles them correctly and that the allocated memory is in fact sufficient and doesn't result in `stack smashing` and other segmentation fault errors.

4. Finally, run memory tests on all the different scenarios to ensure no memory leaks. Corner cases include, but are not limited to, the instances when an argument is detected as invalid *after* some variables have already been allocated---are the allocated variables freed?

