/**
 * @file: crawler.c
 * @author: Amittai J. Wekesa (@siavava)
 * @brief: crawler functionality for TSE
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

// data structures
#include "bag.h"
#include "hashtable.h"
#include "webpage.h"

// memory library
#include "mem.h"

// TSE libraries
#include "pagedir.h"


/*********** Function Prototypes *************/

/**
 * @brief: see function definitions for documentation of functionality and behavior. 
 */

static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);

static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);

static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

static void logr(const char *word, const int depth, const char *url);


/************* GLOBAL CONSTANTS ***************/
// These flags are used to provide exit statuses in the various functions in this file.
const int SUCCESS = 0;
const int INCORRECT_USAGE = -1;
const int EXTERNAL_URL = 1;
const int INVALID_DIRECTORY = 2;


/**
 * @function: main
 * @brief: primary function that receives commandline arguments 
 * and calls relevant secondary functions where necessary.
 * 
 * Inputs:
 * @param argc: argument count. 
 * @param argv: argument vector.
 * 
 * Returns:
 * @return int: an exit status depending on success or errors (see "GLOBAL CONSTANTS" section).
 */
int 
main(const int argc, char* argv[])
{
  /* code */
  char* usage = "./crawler seedURL pageDirectory maxDepth";

  // validate that required number of arguments was provided.
  if (argc < 4) {
    fprintf(stderr, "Incorrect usage: too few arguments!\n");
    fprintf(stderr, "Usage: %s\n", usage);
    exit(INCORRECT_USAGE);
  }
  else if (argc > 4) {
    fprintf(stderr, "Incorrect usage: too many arguments!\n");
    fprintf(stderr, "Usage: %s\n", usage);
    exit(INCORRECT_USAGE);
  }

  // allocate memory for seed URL and page directory
  char** seedURL = mem_calloc(strlen(argv[1]), sizeof(char)); 
  char** pageDirectory = mem_calloc(strlen(argv[2]), sizeof(char));

  // initialize max depth (no need to alloc)
  int maxDepth;

  // parse arguments
  parseArgs(argc, argv, seedURL, pageDirectory, &maxDepth);

  // crawl
  crawl(*seedURL, *pageDirectory, maxDepth);

  // free seedURL and pageDirectory
  free(seedURL);
  free(pageDirectory);
  return SUCCESS;
}


/**
 * @function: parseArgs
 * @brief: processes and validates commandline arguments.
 * IF valid, stores them to memory locations provided by the caller.
 * parseArgs performs no internal memory allocs.
 *
 * DISCLAIMER: 
 * parseArgs expects passed arguments "seedURL" and "pageDirectory" to point to malloc'ed memory.
 * In case of an error, parseArgs frees these two pointers before exiting.
 * Passing in non-malloc'ed memory will cause segmentation
 * faults IF parseArgs encounters an exit condition.
 * 
 * Inputs:
 * @param argc: commandline argument count 
 * @param argv: commandline argument vector 
 * @param seedURL: pointer to (malloc'ed) memory location to save seed URL 
 * @param pageDirectory: pointer to (malooc'ed) memory location to save page directory 
 * @param maxDepth: pointer to statically-allocated (non-malloc'ed) memory location to save max depth
 */
static void 
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{
  // parse the URL
  *seedURL = normalizeURL(argv[1]);
  if (!isInternalURL(*seedURL)) {
    fprintf(stderr, "'%s' is not an internal URL.\n", *seedURL);
    mem_free(seedURL);
    mem_free(pageDirectory);
    exit(EXTERNAL_URL);
  }

  // parse the page directory
  if (pagedir_init(argv[2])) {
    *pageDirectory = argv[2];
  }
  else {
    fprintf(stderr, "Invalid page directory.\n");
    mem_free(seedURL);
    mem_free(pageDirectory);
    exit(INVALID_DIRECTORY);
  }

  // parse maxDepth
  if ( (*maxDepth = atoi(argv[3])) < 0) {
    fprintf(stderr, "max depth cannot be less than ZERO.\n");
    mem_free(seedURL);
    mem_free(pageDirectory);
    exit(-4);
  }
}

/**
 * @function: crawl
 * @brief: runs a depth-first search on webpage.
 * Finds links in start page and follows them to a specified depth limit,
 * indexing all encountered webpages if they fit a specified criterion. 
 * 
 * Inputs:
 * @param seedURL: start URL 
 * @param pageDirectory: directory to save crawl results 
 * @param maxDepth: highest depth to crawl 
 */
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
  while((page = bag_extract(pages_to_crawl)) != NULL) {  

    /*
     * if fetch html of current page succeeds, 
     * save the page to pageDirectory
     */ 
    if (webpage_fetch(page)) {

      
      logr("Fetched", webpage_getDepth(page), webpage_getURL(page));
      
      pagedir_save(page, pageDirectory, documentID++);
      logr("Saved", webpage_getDepth(page), webpage_getURL(page));
      
      /*
      * if page is not at maxDepth, 
      * scan the page for more links to crawl
      */
      if (webpage_getDepth(page) < maxDepth) {
        pageScan(page, pages_to_crawl, pages_seen);
      }
    }
    else {
      fprintf(stderr, "Webpage fetch failed!\n");
    }

    // delete current page
    webpage_delete(page);
  }

  // delete hashtable
  hashtable_delete(pages_seen, NULL);

  // delete bag
  bag_delete(pages_to_crawl, NULL);
}

/**
 * @function: pageScan
 * @brief: Scans a webpage for outward links, 
 * validates and normalizes them,
 * then adds them to the the visit queue.
 * 
 * Inputs:
 * @param page: current page
 * @param pagesToCrawl: bag containing pages to be visited 
 * @param pagesSeen: hashtable containing pages seen (no duplicates!) 
 */
static void
pageScan(webpage_t* page, bag_t* pages_to_crawl, hashtable_t* pages_seen)
{
  logr("Scanning", webpage_getDepth(page), webpage_getURL(page));
  // variable to track position in page
  int pos = 0;

  // reference to returned URL
  char* rawURL;
  while ((rawURL = webpage_getNextURL(page, &pos)) != NULL) {

    // Get normalized URL
    char* url = normalizeURL(rawURL);

    // Free raw URL
    free(rawURL);

    // if URL is internal
    if (isInternalURL(url)) {
      logr("Found", webpage_getDepth(page)+1, url);

      // if successfully entered into pages seen
      if (hashtable_insert(pages_seen, url, "")) {

        int depth = webpage_getDepth(page);

        webpage_t* next_page = webpage_new(url, depth+1, NULL);
        bag_insert(pages_to_crawl, next_page);
        logr("Queued", webpage_getDepth(page)+1, url);
      }
      else {
        logr("IgnDupl", webpage_getDepth(page)+1, url);
        free(url);
      }
    }
    else {
      logr("IgnExtrn", webpage_getDepth(page)+1, url);
      free(url);
    }
  }
}

/**
 * @function: logr -- as provided by Prof. David Kotz
 * @brief: logs crawler progress IF a specified flag is switched on. 
 * 
 * @param word: current operation 
 * @param depth: page depth 
 * @param url: page URL 
 */                                  
static void logr(const char *word, const int depth, const char *url)
{
#ifdef APPTEST
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
#else
  ;
#endif
}
