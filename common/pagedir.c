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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"
#include "pagedir.h"


/************ Function Prototypes ***********/
// void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
// bool pagedir_init(const char* pageDirectory);

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

void
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
  printf("Inside pagedir_save()\n");
  char filepath[sizeof(pageDirectory)+10];
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

    // close the file and return true
    printf("Saved: %s\n", filepath);
    fclose(fp);
  }
  else {
    fprintf(stdout, "Error opening path: '%s'", filepath);
  }
}
