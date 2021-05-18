/**
 * @file querier.c
 * @author Amittai J. Wekesa (@siavava)
 * @brief: This file contains functionality for the TSE querier
 * @version 0.1
 * @date 2021-05-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/*********** HEADER FILES *********/

/* standard libraries */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* memory library */
#include "mem.h"

/* file library */
#include "file.h"

/* Data Structures */
#include "index.h"
#include "webpage.h"

/* Word library */
#include "word.h"

/* query handler */
#include "query.h"

/* page/file IO handler */
#include "pagedir.h"


/*********** FUNCTION PROTOTYPES *********/
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
char** getQuery(FILE* fp);
char*** parseQuery(char** query);
void runQuery(index_t* index, char* pageDirectory, char*** rawQuery);

/* function to log progress */
// static void logProgress(const int depth, const char* operation, const char* item);


/************* GLOBAL CONSTANTS ***************/
/* used to provide exit statuses in the various functions in this file. */
static const int SUCCESS = 0;
static const int INCORRECT_USAGE = 1;
static const int INVALID_DIR = 2;
static const int INVALID_FILE = 3;
static const int INDEX_ERROR = 4;
// static const int INVALID_QUERY = 5;


int 
main(int argc, char* argv[])
{
  /* QUICK TEST */
#ifdef QUICKTEST
  char* testArgs[3] = {"querier", "../data/output/toscrape-1", "../data/output/toscrape-1.index"};
  argc = 3;
  argv = testArgs;
#endif

  /* NORMAL FUNCTIONALITY */  

  /* If invalid number of arguments, print usage and error message, exit non-zero. */
  if (argc != 3) {
    const char* usage = "./indexer [pageDirectory] [indexFilename]\n";

    if (argc < 3) fprintf(stderr, "Too few arguments.\n");
    else fprintf(stderr, "Too many arguments.\n");
    
    fprintf(stderr, "Usage: '%s'", usage);
    exit(INCORRECT_USAGE);
  }

  /* allocate memory for pageDirectory and indexFileName */
  char** pageDirectory = mem_malloc_assert(sizeof(argv[1]), "Memory allocation for pageDirectory failed.");
  char** indexFileName = mem_malloc_assert(sizeof(argv[2]), "Memory allocation for indexFileName failed.");

  /* parse the arguments */
  parseArgs(argv, pageDirectory, indexFileName);

  /* reconstruct the index */
  index_t* index;
  if ( (index = index_load(*indexFileName)) == NULL) {
    fprintf(stderr, "Error initializing index");
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(INDEX_ERROR);
  }

  /* QUERIES */
  // FILE* fp = stdin;
  char** rawQuery;
  while ((rawQuery = getQuery(stdin)) != NULL) { // NULL signifies EOF or error reading from file.

    /* 
     * if first token is NULL
     * the entered query was not valid. 
     * free the returned raw query
     * and continue to next iteration.
     */
    if (rawQuery[0] == NULL) {
      mem_free(rawQuery);
      continue;
    }

    /* parse the query */
    char*** parsedQuery = parseQuery(rawQuery);

    if (parsedQuery != NULL) {
      runQuery(index, *pageDirectory, parsedQuery);
    }

    /* run the query */
    // query_t* results;
    // if ( (results = runQuery(index, *pageDirectory, parsedQuery)) != NULL) {

    //   /* print the results */
    //   query_print(results, stdout);

    //   /* delete the results */
    //   query_delete(results);
    // }
    // else {
    //   fprint(stderr, "An error occured running the query.");
    // }

    /* free the current query before proceeding to next */
    mem_free(parsedQuery);
  }

  /* EXITING */

  /* delete the index */
  index_delete(index);

  /* free commandline inputs */ 
  mem_free(pageDirectory);
  mem_free(indexFileName);

  return SUCCESS;
}

/**
 * @function: parseArgs
 * @brief: parses and interprets the commandline arguments for the indexer 
 * 
 * Inputs:
 * @param argv: argument vector received from commandline 
 * @param pageDirectory: pointer to malloc'ed memory location to hold page directory 
 * @param indexFileName: pointer to malloc'ed memory location to hold path to index file
 * 
 * DISCLAIMER:
 * parseArgs expects input pointers to point to allocated memory.
 * In case of an error, parseArgs frees the memory before exiting.
 * 
 * Passing in non-malloc'ed memory could cause segmentation faults.
 */
static void
parseArgs(char* argv[], char** pageDirectory, char** indexFileName)
{
  /* 
   * If directory does not contain a .crawler file,
   * then it's not a valid crawler directory.
   * Print error message and exit.
   */ 
  if (!pagedir_check(argv[1])) { 
    fprintf(stderr, "'%s' is not a valid crawler directory.\n", argv[1]);
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(INVALID_DIR);
  }
  else {
    *pageDirectory = argv[1];
  }

  /*
   * If unable to open (or create) the file wherein to write the index,
   * print an error message and exit.
   */
  FILE* fp;
  if ((fp = fopen(argv[2], "r")) == NULL) {
    fprintf(stderr, "Invalid index file name and/or directory.\n");
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(INVALID_FILE);
  }
  else {
    fclose(fp);
    *indexFileName = argv[2];
  }
} /* end of parseArgs() */


/**
 * @function: getQuery
 * @brief: reads in query from FILE*
 * returns an array of words in the query, in the specific order.
 * 
 * @param fp: FILE* wherein to read queries
 * @return char**: array of words in the query
 * @return NULL: EOF or error reading from FILE*
 */
char**
getQuery(FILE* fp)
{
  assert(fp != NULL );
  
  /* get query */
  /* if end of file, pass NULL back to caller */
  char* rawQuery;
  if ( (rawQuery = file_readLine(fp)) == NULL) {
    return NULL;
  }

  /* if empty query */
  else if (strlen(rawQuery) == 0) {
    /* print to stderr */
    fprintf(stderr, "Error: empty query.\n");

    /* free the query */
    mem_free(rawQuery);

    /* init temp tokens array to pass back to caller */
    char** tokens = mem_calloc_assert(10, 5*sizeof(char), "Error allocating memory for query tokens.\n");

    /* set first word to NULL to let them know an error occurred. */
    tokens[0] = NULL;

    /* pass tokens array back to caller */
    return tokens;
  }

  /* normalize the query */
  normalizeWord(rawQuery);

  /* to hold split sub-queries */
  char** tokens = mem_calloc_assert(strlen(rawQuery), 5*sizeof(char), "Error allocating memory for query tokens.\n");
  
  /* track position in subquery */
  int pos = 0;

  /* to track last saved token */
  char* lastToken = mem_calloc(100, sizeof(char));

  /* get first word in query */
  char* token;
  if ( (token = strtok(rawQuery, " ")) == NULL) {
    fprintf(stderr, "Error parsing string tokens.\n");
    mem_free(rawQuery);
    mem_free(lastToken);
    tokens[0] = NULL;
    return tokens;
  }

  /* first word cannot be "and" or "or" */
  else if (strcmp(token, "and") == 0 || strcmp(token, "or") == 0) {
    fprintf(stderr, "Error: '%s' at beginning of query.\n", token);
    mem_free(rawQuery);
    mem_free(lastToken);
    tokens[0] = NULL;
    return tokens;
  }

  /* save token if valid */
  tokens[pos++] = mem_malloc(sizeof(token));
  strcpy(tokens[pos-1], token);

  /* track it as last token */
  strcpy(lastToken, token);

  /*
   * while next token is valid, save it.
   * also keep track of the last token.
   */
  while ( (token = strtok(NULL, " ")) != NULL) {
    
    /* if "and" / "or" occur contiguously, query is not valid. */
    if ( (strcmp(token, "and") == 0) || (strcmp(token, "or") == 0) ) {
      if ( (strcmp(lastToken, "and") == 0) || (strcmp(lastToken, "or") == 0) ) {
        fprintf(stderr, "Error: '%s' and '%s' not allowed to follow each other in query.\n", lastToken, token);
        mem_free(rawQuery);
        mem_free(lastToken);
        tokens[0] = NULL;
        return tokens;
      }
    }

    /* else, copy the word into appropriate position and continue */
    tokens[pos++] = mem_malloc(sizeof(token));
    strcpy(tokens[pos-1], token);
    strcpy(lastToken, token);
  }

  /* 
   * if last token is "and" or "or",
   * query is not valid.
   * return NULL
   */
  if ( (strcmp(lastToken, "and") == 0) || (strcmp(lastToken, "or") == 0) ) {
    fprintf(stderr, "Error: '%s' at end of query.\n", lastToken);
    mem_free(rawQuery);
    mem_free(lastToken);
    tokens[0] = NULL;
    return tokens;
  }

  /* free the allocated function variables */
  mem_free(token);
  mem_free(lastToken);
  mem_free(rawQuery);

  /* mark end-point of query */
  tokens[pos++] = NULL;

  /* return the array of tokens */
  return tokens;
} /* end of getQuery() */

/**
 * @function: parseQuery
 * @brief: receives an array of words in a query.
 * splits the query into "actionable" blocks separated by 'OR' statements.
 * 
 * @param query: array of words in a query.
 * @return char*** array of arrays of words --> parts of the query.
 */
char***
parseQuery(char** query)
{
  assert(query != NULL);
  /* alloc memory for split query */
  char*** splitQuery = mem_calloc_assert(2, sizeof(query), "Error allocating memory for parsed query.\n");
  int grouping = 0;
  int pos = 0;

  for(int i=0; ; i++) {

    /* on first NULL (end of query marker), break the loop */
    if (query[i] == NULL) {
      break;
    }

    /* check for "and" sequences */
    if(strcmp(query[i], "and") == 0) {
      ;       /* no action needed */
    }

    /* check for "or" sequences */
    else if(strcmp(query[i], "or") == 0) {
      /* 
       * if 'or',
       * NULL-terminate the current grouping
       * and step to next grouping.
       */
      splitQuery[grouping][pos++] = NULL;
      grouping++;
      pos = 0;
    }

    /* general query words: save it */
    else {
      if (splitQuery[grouping] == NULL) {
        splitQuery[grouping] = mem_malloc(5*sizeof(query));
      }
      splitQuery[grouping][pos++] = mem_calloc(strlen(query[i]) + 5, sizeof(char));
      char* slot = splitQuery[grouping][pos-1];
      char* currentWord = query[i];
      strcpy(slot, currentWord);
    }

    /* free the current word */
    mem_free(query[i]);
  }

  /* free the unsplit query passed in */
  mem_free(query);

  /* mark endpoints with a NULL */
  splitQuery[grouping][pos] = NULL;
  splitQuery[grouping+1] = NULL;

  /* return the 2D array of words */
  return splitQuery;
} /* end of parseQuery() */

/**
 * @brief: function to evaluate split tokens in a query and assemble a query object. 
 * 
 * Inputs:
 * @param index: pointer to a valid index struct
 * @param pageDirectory: page directory wherein the pages are saved.
 * @param rawQuery: sequence of split tokens in a query.
 * 
 * Returns:
 * @return query_t*: pointer to a query struct containing the results.
 */
// query_t* 
void
runQuery(index_t* index, char* pageDirectory, char*** rawQuery)
{
  /* if any param is NULL, return NULL back to caller. */
  if ( (index == NULL) || (pageDirectory == NULL) || (rawQuery == NULL)) {
    return;
  }

  /* build initial subQuery */
  query_t* query = query_build(index, rawQuery[0]);

  /* exit if an error occured. */
  if (query == NULL) {
    fprintf(stderr, "Error creating query.");
    return;
  }

  /* unionize with builds of subsequent subQueries */
  for (int i=1; ; i++) {
    if (rawQuery[i] == NULL) {
      break;
    }
    /* run subQ */
    char** subQuery = rawQuery[i];
    if (query != NULL) {
      query_t* nextQuery = query_build(index, subQuery);
      query = query_union(query, nextQuery);
    }

    /* free subQ */
    mem_free(subQuery);
  }

  /* sort the entries in the query results and load relevant data */
  if (query != NULL) {
    query_index(query, pageDirectory);
    query_print(query, stdout);
    query_delete(query);
  }
}
