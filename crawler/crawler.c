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

// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// web/crawler directories
#include "webpage.h"
#include "pagedir.h"
#include "mem.h"

// data structures
#include "bag.h"
#include "hashtable.h"



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

  // allocate variables
  char* seedURL; 
  char* pageDirectory;
  int maxDepth;

  // parse arguments
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(&seedURL, &pageDirectory, &maxDepth);
  return 0;
}

static void 
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{
  // parse the URL
  *seedURL = normalizeURL(argv[1]);
  if (!isInternalURL(seedURL)) {
    fprintf(stderr, "'%s' is not an internal URL.\n", seedURL);
    exit(-2);
  }

  // parse the page directory
  if (pagedir_init(argv[2])) {
    *pageDirectory = argv[2];
  }
  else {
    fprintf(stderr, "Invalid page directory.\n");
    exit(-3);
  }

  // parse maxDepth
  if ( (*maxDepth = atoi(argv[4])) < 0) {
    fprintf(stderr, "max depth cannot be less than ZERO.\n");
    exit(-4);
  }
}

static void 
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  hashtable_t* pages_seen = hashtable_new(maxDepth);
  bag_t* pages_to_crawl = bag_new();

  bag_insert(pages_to_crawl, seedURL);
  hashtable_insert(pages_seen, 0, seedURL);

  char* url = mem_malloc(1+sizeof(seedURL));
  int depth = 0;
  while( (url = bag_extract(pages_to_crawl)) != NULL) {

    webpage_t* webpage = webpage_new(url, depth, NULL);


  }
}

