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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "mem.h"
#include "webpage.h"
#include "pagedir.h"
#include "index.h"
#include "word.h"


/************** Function Prototypes ***************/
static void parseArgs(const char* argv[], char** pageDirectory, char** indexFileName);
static void indexBuild(const char* pageDirectory, index_t* index);
static void indexPage(webpage_t* page, int docID, index_t* index);



int 
main(int argc, const char* argv[])
{

  const char* testArgs[3];
  testArgs[0] = "indexer";
  testArgs[1] = "../data/output/wikipedia-1";
  testArgs[2] = "../data/output/wikipedia/index";

  char* pageDirectory = mem_malloc_assert(sizeof(argv[1]), "Memory allocation for pageDirectory failed!");
  char* indexFileName = mem_malloc_assert(sizeof(argv[2]), "Memory allocation for indexFileName failed!");
  parseArgs(testArgs, &pageDirectory, &indexFileName);






  // if (argc != 3) {
  //   const char* usage = "./indexer [pageDirectory] [indexFilename]\n";

  //   const char* errMessage;
  //   if (argc < 3) errMessage = "Too few arguments.\n";
  //   else errMessage = "Too many arguments.\n";
    
  //   fprintf(stderr, "%s", errMessage);
  //   fprintf(stderr, "Usage: '%s'", usage);
  //   exit(-1);
  // }

  // char* pageDirectory = mem_malloc_assert(sizeof(argv[1]), "Memory allocation for pageDirectory failed!");
  // char* indexFileName = mem_malloc_assert(sizeof(argv[2]), "Memory allocation for indexFileName failed!");
  // parseArgs(argv, &pageDirectory, &indexFileName);

  
  index_t* index = index_new();
  indexBuild(pageDirectory, index);

  FILE* fp = fopen(indexFileName, "w");
  index_print(index, fp);
  fclose(fp);
  index_delete(index);

  return 0;
}

static void
parseArgs(const char* argv[], char** pageDirectory, char** indexFileName)
{
  strcpy(*pageDirectory, argv[1]);
  if (!pagedir_check(*pageDirectory)) {    
    fprintf(stderr, "Invalid page directory.\n");
    mem_free(*pageDirectory);
    mem_free(*indexFileName);
    exit(-1);
  }

  FILE* fp;
  if ((fp = fopen(argv[2], "w")) == NULL) {
    fprintf(stderr, "Invalid index file name and/or directory.\n");
    mem_free(*pageDirectory);
    mem_free(*indexFileName);
    exit(-2);
  }
   strcpy(*indexFileName, argv[2]);
}

static void 
indexBuild(const char* pageDirectory, index_t* index)
{
  char* directory = mem_malloc_assert(sizeof(pageDirectory)+10, "Error allocating directory");

  for(int docID=1; ; docID++) {

    if (pageDirectory[strlen(pageDirectory)-1] == '/') {
      sprintf(directory, "%s%d", pageDirectory, docID);
    }
    else {
      sprintf(directory, "%s/%d", pageDirectory, docID);
    }

    webpage_t* page;
    if ((page = pagedir_load(directory)) != NULL) {
      indexPage(page, docID, index);
    }
    else {
      mem_free(directory);
      break;
    }
  }
}

static void
indexPage(webpage_t* page, int docID, index_t* index) 
{
  int pos = 0;
  char* word;
  while ((word = webpage_getNextWord(page, &pos)) != NULL) {
    if (strlen(word) > 2) {
      index_insert(index, word, docID);
    }
  }
}
