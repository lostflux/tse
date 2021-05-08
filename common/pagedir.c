/**
 * @file pagedir.c
 * @author Amittai J. Wekesa (@siavava)
 * @brief: directory I/O functionality. 
 * @version 0.2
 * @date 2021-05-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/************** Header Files ****************/

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* file IO */
#include "file.h"

/* memory */
#include "mem.h"

/* data structures */
#include "webpage.h"

/* self */
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

/**
 * @brief see pagedir.h for documentation
 */
bool
pagedir_check(char* dirName)
{
  
  char fullpath[strlen(dirName)+100];

  // generate path to crawler config file.
  if (dirName[strlen(dirName)-1] == '/') {
    sprintf(fullpath, "%s.crawler", dirName);
  }
  else {
    sprintf(fullpath, "%s/.crawler", dirName);
  }

  /*
   * if file exists (hence opens successfully),
   * return true
   */
  FILE* fp;
  if ((fp = fopen(fullpath, "r")) != NULL) {
    fclose(fp);
    return true;
  }

  /*
   * if file fails to open,
   * return false
   */
  return false;
}

/**
 * @brief see pagedir.h for documentation
 */
webpage_t*
pagedir_load(const char* path)
{
  FILE* fp;

  /* if file opens successfully, load the webpage */
  if ((fp = fopen(path, "r")) != NULL) {
    
    // load url
    char* url = file_readLine(fp);
    
    // load page depth where crawler found page
    char* pageDepth = file_readLine(fp);
    int depth = atoi(pageDepth);
    mem_free(pageDepth);

    // load page HTML
    char* html = file_readUntil(fp, NULL);

    // close the file
    fclose(fp);

    // return pointer to a reconstructed webpage.
    return webpage_new(url, depth, html);
  }

  /* if file fails to open, return NULL */
  else {
    return NULL;
  }
}

