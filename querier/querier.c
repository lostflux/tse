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
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* strtok */

/* memory library */
#include "mem.h"

/* file library */
#include "file.h"

/* Data Structures */
#include "index.h"
#include "webpage.h"

/* Word library */
#include "word.h"

#include "query.h"

#include "pagedir.h"


/*********** FUNCTION PROTOTYPES *********/
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
static index_t* indexBuild(const char* indexFileName);
char** getQuery(FILE* fp);
query_t* runQuery(index_t* index, char* pageDirectory, char*** rawQuery);
char*** parseQuery(char** query);

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
  char* testArgs[3] = {"querier", "../data/output/letters-10", "../data/output/letters-10.index"};
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
  if ( (index = indexBuild(*indexFileName)) == NULL) {
    fprintf(stderr, "Error initializing index");
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(INDEX_ERROR);
  }

  /* count the number of pages in the crawler directory. */
  // const int NUMPAGES = pagedir_count(*pageDirectory);


  /* QUERIES */
  FILE* fp = stdin;
  char** rawQuery;
  while ((rawQuery = getQuery(fp)) != NULL) { // NULL signifies EOF or error reading from file.

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

    /* run the query */
    query_t* results;
    if ( (results = runQuery(index, *pageDirectory, parsedQuery)) != NULL) {

      /* index the identified pages per the page directory */
      query_index(results, *pageDirectory);

      /* print the results */
      query_print(results, stdout);

      /* delete the results */
      query_delete(results);
    }

    /* free the current query before proceeding to next */
    mem_free(parsedQuery);
  }

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
  if ((fp = fopen(argv[2], "w")) == NULL) {
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
 * @function: indexBuild
 * @brief: receives an address to a crawler folder 
 * and a pointer to an index_t* object,
 * scans saved webpage files in the directory, 
 * extracting words and inserting them into the index. 
 * 
 * Inputs:
 * @param pageDirectory: page directory to search for saved webpages 
 * @param index: address to file where index is to be written.
 */
static index_t*
indexBuild(const char* indexFileName)
{

  /*
   * if error opening file, return NULL
   */
  FILE* indexFile;
  if ( (indexFile = fopen(indexFileName, "r")) == NULL) {
    fprintf(stderr, "Error accessing index file: '%s'\n", indexFileName);

    /* close input file opened earlier */
    fclose(indexFile);
    return NULL;
  }
  /* log progress */
  // logProgress(0, "output", output);

  /*
   * If error initializing index, return NULL
   */
  index_t* index;
  if ( (index = index_new()) == NULL) {
    return NULL;
  }

  char* rawText = NULL;       // track text found from source file
  char* foundWord = NULL;     // track actual words that are found
  int docID; int count;       // track document ID and counts that are found 
  int pos = 0;                // track position ([x]'th word) in current sentence

  while ( (rawText = file_readWord(indexFile) ) != NULL) {

    /* if word has alphabetical characters */
    if (isalpha(rawText[0]) != 0) {

      /* free previously found word */
      if (foundWord != NULL) {
        mem_free(foundWord);
      }

      //reset position to zero
      pos = 0;

      // normalize the word
      normalizeWord(rawText);

      // allocate word, copy text found
      foundWord = mem_calloc_assert(strlen(rawText), 2*sizeof(char), "Error allocating memory for found word.");

      // copy found word
      strcpy(foundWord, rawText);

      /* log progress */
      // logProgress(2, "word", foundWord);

      // increment position in sentence.
      pos++;
    }
    else if (pos % 2 == 1) {
      
      // parse document ID
      docID = atoi(rawText);

      // increment position in sentence.
      pos++;
    }
    /* if count value, insert pair into index */
    else {
      
      // parse count
      count = atoi(rawText);

      // insert word, document ID, count into the index
      index_set(index, foundWord, docID, count);

      // increment position in the sentence
      pos++;
    }

    /* free the raw text */
    mem_free(rawText);
  }

  /*
   * at end of document (if read was successful), 
   * free last found valid word 
   */
  if (foundWord != NULL) {
    mem_free(foundWord);
  }

  // close the file
  fclose(indexFile);

  /* return re-created index */
  return index;

} /* end of indexBuild() */

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

  /* normalize the query */
  normalizeWord(rawQuery);

  char** tokens = mem_calloc_assert(strlen(rawQuery), 2*sizeof(char), "Error allocating memory for query tokens.");
  int pos = 0;
  char* lastToken = NULL;

  /* get first word in query */
  char* token;
  if ( (token = strtok(rawQuery, " ")) == NULL) {
    fprintf(stderr, "Error: empy query.");
    mem_free(rawQuery);
    tokens[0] = NULL;
    return tokens;
  }

  /* first word cannot be "and" or "or" */
  else if (strcmp(token, "and") == 0 || strcmp(token, "or") == 0) {
    fprintf(stderr, "Error: %s at beginning of word.", token);
    mem_free(rawQuery);
    tokens[0] = NULL;
    return tokens;
  }

  /* save token if valid */
  strcpy(tokens[pos++], token);

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
        fprintf(stderr, "Error: '%s' and '%s' not allowed to follow each other in query.", lastToken, token);
        mem_free(rawQuery);
        tokens[0] = NULL;
        return tokens;
      }
    }

    /* else, copy the word into appropriate position and continue */
    strcpy(tokens[pos++], token);
    strcpy(lastToken, token);
  }

  /* 
   * if last token is "and" or "or",
   * query is not valid.
   * return NULL
   */
  if ( (strcmp(lastToken, "and") == 0) || (strcmp(lastToken, "or") == 0) ) {
    fprintf(stderr, "Error: last token in query cannot be '%s'", lastToken);
    mem_free(rawQuery);
    tokens[0] = NULL;
    return tokens;
  }

  /* free the allocated function variables */
  mem_free(token);

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
  char*** splitQuery = mem_malloc_assert(2*sizeof(query), "Error allocating memory for parsed query");
  
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
      strcpy(splitQuery[grouping][pos++], query[i]);
    }
  }

  /* mark endpoints with a NULL */
  splitQuery[grouping][pos++] = NULL;
  splitQuery[grouping+1] = NULL;

  /* return the 2D array of words */
  return splitQuery;
} /* end of parseQuery() */

query_t* 
runQuery(index_t* index, char* pageDirectory, char*** rawQuery)
{
  /* if any param is NULL, return NULL back to caller. */
  if ( (index == NULL) || (pageDirectory == NULL) || (rawQuery == NULL)) {
    return NULL;
  }

  /* build initial subQuery */
  query_t* query = query_build(index, rawQuery[0]);

  /* unionize with builds of subsequent subQueries */
  for (int i=1; ; i++) {
    if (rawQuery[i] == NULL) {
      break;
    }
    query = query_union(query, query_build(index, rawQuery[i]));
  }

  /* sort the entries in the query results */
  query_index(query, pageDirectory);

  /* return final version of query */
  return query;
}
