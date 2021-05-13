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


/*********** FUNCTION PROTOTYPES *********/
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
static index_t* indexBuild(const char* indexFileName);
char** getQuery(FILE* fp);
int*** runQuery(char** query, index_t* index, char* indexFileName);

/* function to log progress */
static void logProgress(const int depth, const char* operation, const char* item);


/************* GLOBAL CONSTANTS ***************/
/* used to provide exit statuses in the various functions in this file. */
static const int SUCCESS = 0;
static const int INCORRECT_USAGE = 1;
static const int INVALID_DIR = 2;
static const int INVALID_FILE = 3;
static const int INDEX_ERROR = 4;
static const int INVALID_QUERY = 5;

static const char* __WRONG_QUERY = "?";


int 
main(int argc, const char* argv[])
{
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
  if ( (index = indexBuild(indexFileName) == NULL)) {
    fprintf(stderr, "Error initializing index");
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(INDEX_ERROR);
  }

  /* count the number of pages in the crawler directory. */
  const int NUMPAGES = pagedir_count(*pageDirectory);


  /* QUERIES */
  FILE* fp = stdin;
  char** query;
  while ( (query = getQuery(fp)) != NULL) {

    /* if first token is NULL, no word was entered. */
    if (query[0] == NULL) {
      fprintf(stderr, "Error: empty query. Please enter at least one word.");
      mem_free(query);
      continue;
    }

    /* if first token is __WRONG_QUERY, an invalid query was entered. */
    else if (strcmp(query[0], __WRONG_QUERY) == 0) {
      fprintf(stderr, "Error: reserved words 'and', 'or' not allowed at start, end, or following each other.");
      mem_free(query);
      continue;
    }

    /* process the query */
    runQuery(query, index, *pageDirectory);


  }


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

  /* log progress */
  logProgress(0, "dir", *pageDirectory);
  logProgress(0, "index", *indexFileName);
}

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
}

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

  char* tokens[] = mem_calloc_assert(strlen(rawQuery), 2*sizeof(char), "Error allocating memory for query tokens.");
  int pos = 0;
  char* lastToken = NULL;

  /* get first word in query */
  char* token;
  if ( (token = strtok(rawQuery, " ")) == NULL) {
    mem_free(rawQuery);
    tokens[1] = NULL;
    return tokens;
  }

  /* first word cannot be "and" or "or" */
  else if (strcmp(token, "and") == 0 || strcmp(token, "or") == 0) {
    mem_free(rawQuery);
    tokens[0] == __WRONG_QUERY;
    return tokens;
  }

  /* save token */
  strcpy(tokens[pos++], token);
  strcpy(lastToken, token);

  /*
   * while next token is valid, save it.
   * also keep track of the last token.
   */
  while ( (token = strtok(NULL, " ")) != NULL) {
    

    /* if "and" / "or" occur contiguously, query is not valid. */
    if ( (strcmp(token, "and") == 0) || (strcmp(token, "or") == 0) ) {
       if ( (strcmp(lastToken, "and") == 0) || (strcmp(lastToken, "or") == 0) ) {
         mem_free(lastToken);
         mem_free(rawQuery);
        tokens[0] == __WRONG_QUERY;
        return tokens;
      }
    }

    /* else, copy the word into appropriate position and continue */
    strcpy(tokens[pos++], token);
    strcpy(lastToken, token);
  }

  if ( (strcmp(lastToken, "and") == 0) || (strcmp(lastToken, "or") == 0) ) {
    mem_free(lastToken);
    mem_free(rawQuery);
    tokens[0] = __WRONG_QUERY;
    return tokens;
  }

  /* free the allocated function variables */
  mem_free(lastToken);
  mem_free(token);

  assert(tokens != NULL);

  /* mark end-point of query */
  tokens[pos++] = NULL;

  /* return the array of tokens */
  return tokens;
}


char***
parseQuery(char** query)
{
  char*** splitQuery = mem_malloc_assert(2*sizeof(query), "Error allocating memory for parsed query");
  
  int grouping = 0;
  int marker = 0;
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
      // if 'or', step to next grouping! 
      grouping++;
      marker=0;
    }

    /* general query words: save it */
    else {
      strcpy(splitQuery[grouping][marker++], query[i]);
    }
  }

  /* mark endpoint with a NULL */
  splitQuery[grouping+1] = NULL;

  return splitQuery;

}


int***
runQuery(char** query, index_t* index, char* indexFileName)
{
  /* make sure all parameters are valid */
  assert(query != NULL);
  assert(index != NULL);
  assert(indexFileName != NULL);

  /* int [] [] [] */
  int*** results = mem_malloc_assert(1000*sizeof(int), "Error allocating memory for results in runQuery");
  int pos = 0;
  /* parse query */
  for (int i=1; ; i++) {
    if (query[i] == NULL) {
      break;
    }
    int** ranks = index_rank(index, query[i], indexFileName);
    results[pos++] = ranks;    
  }
  results[pos+1] = NULL;

  return results;
}

int**
intersection(int*** ranks)
{
  int** intersection = mem_malloc_assert(sizeof(ranks), "Error allocating memory in intersection");
  int* keys = intersection[0];
  int* values = intersection[1];

  for(int i=0; ; i++) {
    // NULL-terminated query
    if (ranks[i] == NULL) {
      break;
    }
    if (i = 0) {
      for (int j=0; ; j++) {
        if (ranks[i][0][j] == NULL) {

          /* append NULL termination and exit */
          keys[j] = NULL;
          values[j] = NULL;
          break;
        }
        keys[j] = ranks[i][0][j];
        values[j] = ranks[i][1][j];
      }
    }
    else {
      for (int j=0; ; j++) {
        if (ranks[i][0][j] == NULL) {
          break;
        }
        int currentKey = ranks[i][0][j];
        int currentCount = ranks[i][1][j];
        for (int pair=0; ; pair++) {
          if (intersection[pair] == NULL) {
            break;
          }
          // if ()

        }

      }
    }
  }
}
