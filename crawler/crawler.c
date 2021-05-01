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
// #include "crawler.h"

// data structures
#include "bag.h"
#include "hashtable.h"



/*********** Function Prototypes *************/
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);



int 
main(const int argc, char* argv[])
{
  /* code */
  char* usage = "./crawler seedURL pageDirectory maxDepth";

  // temporary test
  if (argc == 1) {
    char* seedURL; 
    char* pageDirectory;
    int maxDepth;

    char* testArguments[4];
    testArguments[0] = "./crawler";
    testArguments[1] = "http://cs50tse.cs.dartmouth.edu/tse/letters/";
    testArguments[2] = "../data/output/letters-10";
    testArguments[3] = "10";

    // parse arguments
    parseArgs(argc, testArguments, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);
    return 0;
  }

  if (argc < 4) {
    fprintf(stderr, "Incorrect usage: too few arguments!\n");
    fprintf(stderr, "Usage: %s\n", usage);
    exit(-1);
  }
  else if (argc > 4) {
    fprintf(stderr, "Incorrect usage: too many arguments!\n");
    fprintf(stderr, "Usage: %s\n", usage);
    exit(-1);
  }

  char* seedURL; 
  char* pageDirectory;
  int maxDepth;

  // parse arguments
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);
  return 0;
}


static void 
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{
  // parse the URL
  *seedURL = normalizeURL(argv[1]);
  if (!isInternalURL(*seedURL)) {
    fprintf(stderr, "'%s' is not an internal URL.\n", *seedURL);
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
  if ( (*maxDepth = atoi(argv[3])) < 0) {
    fprintf(stderr, "max depth cannot be less than ZERO.\n");
    exit(-4);
  }
}


static void 
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  int currentDepth = 0;

  // create hashtable to track pages seen
  hashtable_t* pages_seen = hashtable_new(maxDepth);
  
  // insert seedURL into pages seen
  hashtable_insert(pages_seen, seedURL, "");

  // create bag to store pages to visit
  bag_t* pages_to_crawl = bag_new();

  // create webpage for start URL
  webpage_t* startpage = webpage_new(seedURL, currentDepth, NULL);

  // insert start page into bag of pages to crawl
  bag_insert(pages_to_crawl, startpage);

  // variable to track document ID
  int documentID = 1;

  // variable to track current page
  webpage_t* page;

  // loop until bag of pages to visit is empty...
  while( (page = bag_extract(pages_to_crawl)) != NULL) {

    /*
     * if fetch html of current page succeeds, 
     * save the page to pageDirectory
     */ 
    if (webpage_fetch(page)) {

      // #ifdef DEBUG
      printf("Fetched: %s\n", webpage_getURL(page));
      // #endif
      pagedir_save(page, pageDirectory, documentID);
      documentID++;
    }
    else {
      fprintf(stderr, "Webpage fetch failed!\n");
    }

    /*
     * if page is not at maxDepth, 
     * scan the page for more links to visit
     */

    if (webpage_getDepth(page) < maxDepth) {
      pageScan(page, pages_to_crawl, pages_seen);
    }
    else {
      printf("exited\n");
    }

    // delete current page
    webpage_delete(page);
  }

  // delete hashtable
  hashtable_delete(pages_seen, NULL);

  // delete bag
  bag_delete(pages_to_crawl, webpage_delete);
}



static void
pageScan(webpage_t* page, bag_t* pages_to_crawl, hashtable_t* pages_seen)
{
  printf("Scanning: %s\n", webpage_getURL(page));
  // variable to track position in page
  int pos = 0;

  // reference to returned URL
  char* url;
  while ((url = webpage_getNextURL(page, &pos)) != NULL) {
    // if URL is internal
    if (isInternalURL(url)) {
      printf("Found: %s\n", url);

      // if successfully entered into pages seen
      if (hashtable_insert(pages_seen, url, "")) {

        int depth = webpage_getDepth(page);
        webpage_t* next_page = webpage_new(url, depth+1, NULL);
        bag_insert(pages_to_crawl, next_page);
        printf("Inserted: %s\n", url);
      }
    }
    free(url);
  }
}

