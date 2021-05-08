# CS50 TSE Indexer

## Implementation Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.

Here we focus on the core subset:

- Data structures
- Control flow: pseudo code for overall flow, and for each of the functions
- Detailed function prototypes and their parameters
- Error handling and recovery
- Testing plan

***

## Data structures

- We use a specialised data structure; an 'index' which masks and modifies functionality of a hashtable.
- The hashtable itself implements an hashed-access array of sets.
- Each set holds a pairing of a key to a related item.
- For the 'index' implementation, each 'key' is a word and the associated 'item' is a document-ID to count pairing that lists the number of times the word was found in each document related to a webpage.
- As standard practice, we do not save ZERO counts to the counter.

The size of the hashtable (slots) is impossible to determine in advance, so we use **200**.

The index data structure is inplemented in [index.c](../common/index.c) and exported through [index.h](../common/index.h).

***

## Control flow

The Indexer is implemented in one file `indexer.c`, with five functions:

### main

The `main` function simply allocates needed variables then calls `parseArgs` and `indexBuild`, and cleans up before exiting.
The program has the following exit flags:

```c
/* used to provide exit statuses in the various functions in this file. */
static const int SUCCESS = 0;
static const int INCORRECT_USAGE = 1;
static const int INVALID_DIR = 2;
static const int INVALID_FILE = 3;
```

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

- for `pageDirectory`, ensures it's a valid crawler directory by checking for presence of a `.crawler` file. This is done by calling `pagedir_check()`.
- for `indexFileName`, checks for ability to create and/or write to the file.

### indexBuild

Does the work of generating sequences of files and polling them to see if they exist, before calling `indexPage` to index the words found in the page.

Pseudocode:

```pseudocode
step through document ID's, counting from zero upwards.
  generate path to potential crawler file.
  attempt to load webpage from file.
  if page load succeeds,
    call indexPage on that page.
  if page load fails,
    end of files in directory. BREAK and exit.
```

### indexPage

This function implements the *indexPage* mentioned in the design.
Given a `webpage`, scan the page to extract any words, ignoring "trivial" words *of length less than three characters*; for any words found, insert them into the index, keeping track of the unique identifier of the document wherein they were found.

Pseudocode:

```pseudocode
while word from page is not NULL,
  if word is has 3 or more characters;
    normalize word, converting it to lower case.
    insert word into index.
    get next word.
```

***

## Other modules

### pagedir

We create a re-usable module `pagedir.c` to handles the *pagesaver*  mentioned in the design (writing a page to the pageDirectory), and marking it as a Crawler-produced pageDirectory (as required in the spec).
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to initialize and validate a pageDirectory, and how to write and read page files, in one place... anticipating future use by the Indexer and Querier.

#### Pseudocode for `pagedir_init`

  construct the pathname for the .crawler file in that directory
  open the file for writing; on error, return false.
  close the file and return true.

#### Pseudocode for `pagedir_save`

  construct the pathname for the page file in pageDirectory
  open that file for writing
  print the URL
  print the depth
  print the contents of the webpage
  close the file

#### Pseudocode for `pagedir_check`

  attempt to open the `.crawler` file in the directory.
  if the open is successful, return `true`.
  if the open fails, return `false`.

#### Pseudocode for `pagedir_load`

  attempt to open file at provided path.
  if open is successful;
  read in the first sentence in the file: the page's url.
  read in the second sentence in the file: the page's depth.
  read in the rest of the file: the page's HTML document.
  close the file.
  create a new webpage with the data.
  return a pointer to the created webpage.

### libcs50

We leverage the modules of libcs50, most notably `bag`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs; in that regard, it serves as the *pagefetcher* described in the design.
Indeed, `webpage_fetch` enforces the 1-second delay for each fetch, so our crawler need not implement that part of the spec.

***

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
static void indexBuild(const char* pageDirectory, index_t* index);
static void indexPage(webpage_t* page, int docID, index_t* index);
static void logProgress(const int depth, const char* operation, const char* item);
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_check(char* dirName);
webpage_t* pagedir_load(const char* filepath);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions and `<assert.h>` library functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_init` returns false if there is any trouble creating the `.crawler` file, allowing the Crawler to decide what to do; the `webpage` module returns false when URLs are not retrievable, and the Crawler does not treat that as a fatal error.
`pagedir_load` returns `NULL` if creation of a webpage fails, allowing the Indexer to decide on a course of action.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

There are four units (indexer, pagedir, index, and word).

The `index` is tested using a routine defined in `indextest.c`, which reads in data from a previously generated indexer file, re-writing it out to a new file, and comparing the two files for discrepancies.
Two shell scripts, `indextest.sh` and `indextest_memcheck.sh`, check the dunctionality and memory usage of `indextest.c`. See `indextest.out` and `indextest_memcheck.out` for typical output.

The `indexer` is tested with `testing.sh`, which passes in directories of previously generated Crawler directories and output files.
See `testing.out` for typical output.

Another shell script, `memcheck.sh`, analyses memory usage of the Indexer.
See `memcheck.out` for typical output.

The smaller `word` module is tested in its usage in `index.c` and `indextest.c`.

The crawler represents the whole system and is covered below.
The pagedir unit is tiny; it could be tested using a small C 'driver' to invoke its functions with various arguments, but it is likely sufficient to observe its behavior during the system test.

### Regression testing

The indexer doesn't take much time to run on directories. However, the back-end runs a lot of dynamic memory allocations; running memory checks can take up to five minutes to complete.

For routine regression tests, we index the `../data/outputs/` directory, saving word counts to `*.index` file in the same directory.

> For Lab 5, you are not required to script regression tests, though you may find the technique useful for your own testing/debugging process.

### Integration/system testing

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments.
The `indexer` is tested with `testing.sh`, which passes in directories of previously generated Crawler directories and output files.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.

See `testing.out` for typical output.

Another shell script, `memcheck.sh`, analyses memory usage of the Indexer.

See `memcheck.out` for typical output.

Run that script with `bash -v testing.sh` so the output of indexer is intermixed with the commands used to invoke the crawler.
Verify correct behavior by studying the output, and by sampling the files created in the respective pageDirectories.

> For Lab 5, as noted in the assignment, you may submit a smaller test run.
> Furthermore, we recommend turning off detailed logging output for these tests, as they may `testing.out` rather large!
