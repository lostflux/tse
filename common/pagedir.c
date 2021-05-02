/**
 * @file pagedir.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/************** Header Files ****************/

// standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// memory
#include "mem.h"

// data structures
#include "webpage.h"

// self
#include "pagedir.h"


/**
 * @brief see pagedir.h for documentation
 */
bool 
pagedir_init(const char* pageDirectory)
{
  char crawlerConfig[sizeof(pageDirectory)+100];
  FILE* fp;
  sprintf(crawlerConfig, "%s/.crawler", pageDirectory);
  if ( (fp = fopen(crawlerConfig, "w") ) != NULL) {
    fclose(fp);
    return true;
  }
  else {
    fprintf(stderr, "Error initializing: %s\n", crawlerConfig);
  }
  return false;
}

/**
 * @brief see pagedir.h for documentation
 */
void
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
  char filepath[strlen(pageDirectory) + 10];
  FILE* fp;
  sprintf(filepath, "%s/%d", pageDirectory, docID);
  if ( (fp = fopen(filepath, "w") ) != NULL) {

    // Get page url, html and depth
    int depth = webpage_getDepth(page);
    char* url = webpage_getURL(page);
    char* html = webpage_getHTML(page);

    // print the data to file
    fprintf(fp, "%s\n", url);
    fprintf(fp, "%d\n", depth);
    fprintf(fp, "%s\n", html);

    // close the file
    fclose(fp);
  }
  else {
    fprintf(stderr, "Error opening path: '%s'", filepath);
  }
}
