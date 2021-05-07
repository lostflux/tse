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

/* data structures */
#include "webpage.h"
#include "index.h"

/* memory library */
#include "mem.h"

/* TSE libraries */
#include "pagedir.h"
#include "word.h"


/************** Function Prototypes ***************/

/**
 * @function: parseArgs
 * @brief: parses the commandline arguments and interprets them for the indexer 
 * 
 * @param argv 
 * @param pageDirectory 
 * @param indexFileName 
 */
static void parseArgs(char* argv[], char** pageDirectory, char** indexFileName);
static void indexBuild(const char* pageDirectory, index_t* index);
static void indexPage(webpage_t* page, int docID, index_t* index);

/* function to log progress */
static void logProgress(const int depth, const char* operation, const char* item);



int 
main(int argc, char* argv[])
{

/* QUICKTEST MODULE */
#ifdef QUICKTEST
  char* testArgs[3];
  testArgs[0] = "indexer";
  testArgs[1] = "../data/output/wikipedia-1";
  testArgs[2] = "../data/output/wikipedia-1.index";

  char** pageDirectory = mem_malloc_assert(sizeof(testArgs[1]), "Memory allocation for pageDirectory failed.");
  char** indexFileName = mem_malloc_assert(sizeof(testArgs[2]), "Memory allocation for indexFileName failed.");
  parseArgs(testArgs, pageDirectory, indexFileName);

  index_t* index = index_new();
  indexBuild(*pageDirectory, index);

  FILE* fp = fopen(*indexFileName, "w");
  index_print(index, fp);
  logProgress(0, "Built", *indexFileName);
  fclose(fp);

  index_delete(index);
  logProgress(0, "Deleted", "index object.");

  mem_free(pageDirectory);
  mem_free(indexFileName);

  return 0;
#else
/* NORMAL FUNCTIONALITY */  

  /* If invalid number of arguments, print usage and error message, exit non-zero. */
  if (argc != 3) {
    const char* usage = "./indexer [pageDirectory] [indexFilename]\n";

    if (argc < 3) fprintf(stderr, "Too few arguments.\n");
    else fprintf(stderr, "Too many arguments.\n");
    
    fprintf(stderr, "Usage: '%s'", usage);
    exit(-1);
  }

  char** pageDirectory = mem_malloc_assert(sizeof(argv[1]), "Memory allocation for pageDirectory failed.");
  char** indexFileName = mem_malloc_assert(sizeof(argv[2]), "Memory allocation for indexFileName failed.");
  parseArgs(argv, pageDirectory, indexFileName);

  index_t* index = index_new();
  indexBuild(*pageDirectory, index);

  FILE* fp = fopen(*indexFileName, "w");
  index_print(index, fp);
  logProgress(1, "Printed", *indexFileName);
  fclose(fp);

  index_delete(index);
  logProgress(0, "Deleted", "index object.");

  mem_free(pageDirectory);
  mem_free(indexFileName);

  return 0;
#endif
}

static void
parseArgs(char* argv[], char** pageDirectory, char** indexFileName)
{
  if (!pagedir_check(argv[1])) { 
    
    fprintf(stderr, "'%s' is not a valid crawler directory.\n", argv[1]);
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(-1);
  }
  *pageDirectory = argv[1];

  FILE* fp;
  if ((fp = fopen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Invalid index file name and/or directory.\n");
    mem_free(pageDirectory);
    mem_free(indexFileName);
    exit(-2);
  }
  fclose(fp);
  *indexFileName = argv[2];

  /* log progress */
  logProgress(0, "dir", *pageDirectory);
  logProgress(0, "index", *indexFileName);
}

static void 
indexBuild(const char* pageDirectory, index_t* index)
{  
  logProgress(2, "START", "\n");
  for (int docID=1; ; docID++) {

    char extension[20];

    if (pageDirectory[strlen(pageDirectory)-1] == '/') {
      sprintf(extension, "%d", docID);
    }
    else {
      sprintf(extension, "%s%d", "/", docID);
    }

    // char* directory = mem_malloc_assert(sizeof(pageDirectory)+2, "Error allocating directory");
    char directory[strlen(pageDirectory) + strlen(extension)];
    sprintf(directory, "%s%s", pageDirectory, extension);

    logProgress(3, "file", directory);

    webpage_t* page;
    if ((page = pagedir_load(directory)) != NULL) {
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

static void
indexPage(webpage_t* page, int docID, index_t* index) 
{
  int pos = 0;
  char* word;
  while ( (word = webpage_getNextWord(page, &pos)) != NULL) {
    if (strlen(word) > 2) {
      normalizeWord(word);
      index_insert(index, word, docID);
    }
    mem_free(word);
  } 
}


static void 
logProgress(const int depth, const char* operation, const char* item)
{
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