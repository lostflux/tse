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


/**
 * @function: pagedir_count
 * @brief: counts the number of valid webpages
 * saved in a page directory
 * 
 * DISCLAIMER: This function does not check for validity of the directory
 * as a crawler directory.
 * The caller is responsible for having ascertained validity.
 * 
 * @param pageDirectory: path to a valid crawler directory
 * 
 * @return int: number of valid pages in the crawler directory
 */
int
pagedir_count(const char* pageDirectory)
{

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

    // load webpage from address. If null, break loop.
    webpage_t* page;
    if ((page = pagedir_load(directory)) != NULL) {

      // delete the page
      webpage_delete(page);
    }

    // once pages are out of range, return last successful page ID
    else {
      return docID-1;
    }
  }
}