/**
 * @file: crawler.c
 * @author: Amittai J. Wekesa (@siavava)
 * @brief: File containing functionality of crawler for Tiny Search Engine
 * @version: 0.1
 * @date: 2021-04-28
 * 
 * @copyright: Copyright (c) 2021
 * 
 */

/***************** Header Files ***************/
#include <stdio.h>
#include <stdlib.h>
#include "webpage.h"
#include "pagedir.h"

/*********** Function Prototypes *************/
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
// static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);



int 
main(int argc, const char* argv[])
{
  /* code */
  char* usage = "./crawler seedURL pageDirectory maxDepth";
  if (argc < 4) {
    fprintf(stderr, "Incorrect usage: too few arguments!\n");
    fprintf(stderr, "%s\n", usage);
    exit(-1);
  }
  else if (argc > 4) {
    fprintf(stderr, "Incorrect usage: too many arguments!\n");
    fprintf(stderr, "%s\n", usage);
    exit(-1);
  }

  /**
   * @brief Assumptions:
   *      max size of seedURL = 50 characters
   *      max size of pageDirectory = 50 characters
   * 
   */
  char* seedURL = malloc(50*sizeof(char*));
  char* pageDirectory = malloc(50*sizeof(char));
  int maxDepth;
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  return 0;
}

static void 
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{
  // parse the URL
  strcpy(seedURL, argv[1]);
  normalizeURL(seedURL);
  if (!isInternalURL(seedURL)) {
    exit(1);
  }

  // parse the page directory
  strcpy(pageDirectory, argv[2]);
  pagedir_init(pageDirectory);

  // parse maxDepth
  *maxDepth = atoi(argv[4]);
  if (maxDepth < 0) {
    fprintf(stderr, "max depth cannot be less than ZERO.\n");
    exit(2);
  }
}

