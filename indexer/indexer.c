/**
 * @file indexer.c
 * @author Amittai J. Wekesa (@siavava)
 * @brief contains functionality for TSE indexer
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/************** Header Files ***************/
/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/* data structures */
#include "webpage.h"
#include "index.h"

/* memory library */
#include "mem.h"

/* TSE libraries */
#include "pagedir.h"
#include "word.h"


/************** Function Prototypes ***************/

/* See function definitions for documentation */
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
static void indexBuild(const char* pageDirectory, index_t* index);
static void indexPage(webpage_t* page, int docID, index_t* index);

/* function to log progress */
static void logProgress(const int depth, const char* operation, const char* item);


/************* GLOBAL CONSTANTS ***************/
/* used to provide exit statuses in the various functions in this file. */
static const int SUCCESS = 0;
static const int INCORRECT_USAGE = 1;
static const int INVALID_DIR = 2;
static const int INVALID_FILE = 3;


int 
main(int argc, char* argv[])
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

  char** pageDirectory = mem_malloc_assert(sizeof(argv[1]), "Memory allocation for pageDirectory failed.");
  char** indexFileName = mem_malloc_assert(sizeof(argv[2]), "Memory allocation for indexFileName failed.");
  parseArgs(argv, pageDirectory, indexFileName);

  index_t* index = index_new();
  indexBuild(*pageDirectory, index);

  FILE* fp = fopen(*indexFileName, "w");
  if (fp != NULL) {
    index_print(index, fp);
    logProgress(1, "printed", *indexFileName);
    fclose(fp);
  }

  index_delete(index);
  logProgress(0, "deleted", "index object.");

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
static void 
indexBuild(const char* pageDirectory, index_t* index)
{

  /* log progress */
  logProgress(2, "START", "\n");

  /*
   * Step through docID's from 1
   * since docID's are assigned incrementally by the crawler.
   * Once a non-existent file is reached, break.
   */
  for (int docID=1; ; docID++) {

    // Initialize extension.
    char extension[20];

    // Create extension (varies on presence/absence of a slash at end) 
    if (pageDirectory[strlen(pageDirectory)-1] == '/') {
      sprintf(extension, "%d", docID);
    }
    else {
      sprintf(extension, "%s%d", "/", docID);
    }

    // append extension
    char directory[strlen(pageDirectory) + strlen(extension)];
    sprintf(directory, "%s%s", pageDirectory, extension);

    /* log progress */
    logProgress(3, "file", directory);

    // load webpage from address. If null, break loop.
    webpage_t* page;
    if ((page = pagedir_load(directory)) != NULL) {

      /* log progress */
      logProgress(4, "page", webpage_getURL(page));

      // index the page
      indexPage(page, docID, index);

      // delete the page
      webpage_delete(page);
    }
    else {
      logProgress(2, "END", "\n");
      break;
    }
  }
}

/**
 * @function: indexPage
 * @brief: receives a single webpage struct and scans it for words,
 * normalizing them and inserting them into the index.
 * 
 * @param page: pointer to webpage to search for words.
 * @param docID: depth of page (as discovered by the crawler).
 * @param index. pointer to index struct wherein to save the encountered words.
 */
static void
indexPage(webpage_t* page, int docID, index_t* index) 
{
  assert(page != NULL);
  assert(index != NULL);

  int pos = 0;                                                    // variable to track position in webpage
  char* word;                                                     // variable to reference returned word
  while ( (word = webpage_getNextWord(page, &pos)) != NULL) {     // while next word from page is not NULL
    if (strlen(word) > 2) {                                       // if word is longer than two characters...
      normalizeWord(word);                                        // normalize the word. (defined in word.c)
      index_insert(index, word, docID);                           // insert word into index.
    }
    mem_free(word);                                               // free pointer from webpage_getNextWord()
  } 
}

/* Function to log progress */
static void 
logProgress(const int depth, const char* operation, const char* item)
{
/*
 * Checks for a flag and logs progress of program to stdout.
 * Different cases for start/continuing/end.
 */
#ifdef LOGPROGRESS
  if (strcmp(operation, "START") == 0) {
    printf("%s %*s%7s\n", item, 2*depth, "  ", operation);
  }
  else if (strcmp(operation, "END") == 0) {
    printf("%*s%7s %s\n", 2*depth, "  ", operation, item);
  }
  else {
    printf("%*s%7s: %s\n", 2*depth, "  ", operation, item);
  }
#else
  ;
#endif
}