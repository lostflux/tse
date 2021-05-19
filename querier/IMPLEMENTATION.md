# CS50 TSE Querier

## IMPLEMENTATION

Author: Amittai J. Wekesa (github: @siavava)

***


In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.

***
## Data structures 

We use two main data structures: a `query` to track query results and perform operations, and an `index` to hold page indexes.

The query data structure also uses another data type, `webpage`, to store, retrieve, and manipulate pages.

In our querier, we assume that the maximum number of pages in the index is `100`. This can easily be modified by changing the `MAXPAGES` global constant in the [query](query.c) data structure.

***

## Control flow

The Querier is implemented in one directory, [querier](../querier), albeit in three files---[querier](querier.c) to handle the pure logic of getting queries from the user and calling relevant functions to manipulate it and give feedback back to the user, and [query](query.c) to handle back-end query handling and data structures.


### main

The `main` function simply calls `parseArgs`, `getQuery`, `parseQuery`, and `runQuery`, then exits zero.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.
* for `pageDirectory`, check if it is a valid crawler-generated directory.
* for `indexFileName`, ensure it is an existent file that can be opened for reading.
* if any trouble is found, print an error to stderr, free previously allocatted variables, and exit non-zero.

### getQuery

Does the work of reading in queries from the user.

Given a FILE pointer, invokes a helper function from [file](../libcs50/file.h) to read in lines from the file and process them. Splits the sentence into individual words and stores them in an array oc words (`char**`).
Signifies back if an invalid query is received, e.g. if `and` and `or` occur in successive words or at the beginning or end of the query.


### parseQuery

Does the work of interpreting queries.

Given an array of words, it scans through, checking for `AND` and `OR` breakpoints. It then breaks the queries on `OR` sequences into a number of sub-queries and returns an array of these sub-queries (`char***`).

### runQuery

Given an array of sub-queries, it runs the query, creating intersections of qords in each sub-query then creating unions of the separate sub-queries.

```pseudocode=
Start
loop over subqueries
  loop over words in each subquery
    create intersection of query resultswords within sub-query
  create union of subquery results with the other subqueries. 
```

***

## Other modules

### query

Provides a data abstraction to store and manipulate query results.


### libcs50

We leverage the modules of libcs50, most notably `counters` and `webpage` in the `query`, and `hashtable` in the index.

***
## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in [querier.c](./querier.c) and is not repeated here.

```c=
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
char** getQuery(FILE* fp);
char*** parseQuery(char** query);
void runQuery(index_t* index, char* pageDirectory, char*** rawQuery);
static void prompt(void);
```

### query

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in [query.c](./query.c) and [query.h](./query.h), and is not repeated here.

```c=
/************ Exported Functions ***********/
typedef struct query query_t;
query_t* query_new();
void query_delete(query_t* query);
query_t* query_build(index_t* index, char** words);
void query_intersection(index_t* index, query_t* query, char* nextWord);
query_t* query_union(query_t* subQuery1, query_t* subQuery2);
void query_index(query_t* query, char* pageDirectory);
void query_print(query_t* query, FILE* fp);
counters_t* query_getCounters(query_t* query);


/*********** Static Function Prototypes *********/
static void dubCounters(void* arg, int docID, int count);
static void intersect(void* arg, int docID, int count);
static void unite(void* arg, int docID, int count);
static void sort(void* arg, int docID, int count);
static void setZero(void* arg, int docID, int count);
```

***

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example:

* `getQuery` returns NULL on no input. This is handles by main and does not cause an exit.
* `runQuery` does not exit on erratic queries. Instead, it skips execution and returns control back to the caller.
* the [query](query.c) library returns `NULL` structs if a word is not found. These are handled as appropriate.

***

## Testing plan

Here is a testing plan for the querier:

### Unit testing

There are only two units (querier and query).
The [querier](./querier.c) represents the whole system and is covered below, while the [query](./query.c) offers core functionality for the querier, and is thus indirectly tested through usage in the querier.

### Regression testing

For routine regression tests, we query the `toscrape` directory/index generated with maxtest 1. The index is not too large, but it is big enough to be helpful.

To run the querier with the default `toscrape` directory, run `make quicktest`.

### Integration/system testing

* We write a script [testing.sh](./testing.sh) that invokes the [fuzzquery](./fuzzquery.c) several times and pipes random queries generated by fuzzquery into the querier. We also explicitly attempt incorrect usage in the testing script.
* Second, a run with valgrind over the same calls to detect any memory leaks. These tests are defined in [memcheck.sh](./memcheck.sh), and can take a while to complete.