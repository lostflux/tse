/**
 * @file pagedir.h
 * @author Amittai J. Wekesa (@siavava)
 * @brief: directory IO -- exports functionality from pagedir.c 
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef __PAGEDIR_H

#define __PAGEDIR_H

/*********** Header Files ************/

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* Data Structures */
#include "webpage.h"

// typedef struct webpage webpage_t;

/**
 * @function: pagedir_save
 * @brief: saves a given webpage's reference (link and depth) 
 * and contents to a file in a specified directory.
 * Files are saved with a specified document ID as filename 
 * 
 * Inputs:
 * @param page: webpage to save 
 * @param pageDirectory: save directory 
 * @param docID: ID to save current page as
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**
 * @function: pagedir_init
 * @brief: initializes a specified directory for saving webpages.
 * Adds a '.crawler' file as an identifier that the directory is a crawler directory
 * DISCLAIMER: expects an existing directory.
 * 
 * Inputs:
 * @param pageDirectory: The directory to initialize as a crawler directory
 * 
 * Returns
 * @return true: initialization success 
 * @return false: initialization failure 
 */
bool pagedir_init(const char* pageDirectory);


/**
 * @function: pagedir_check
 * @brief: checks if a specified directory is a crawler directory.
 * Polls for existence of ".crawler" file.
 * 
 * @param dirName: directory wherein to check for crawler files.
 * @return true: valid crawler directory.
 * @return false: not a valid crawler directory. 
 */
bool pagedir_check(char* dirName);


/**
 * @function: pagedir_load
 * @brief: reads data from a crawler-generated file and reconstructs the webpage.
 * DISCLAIMER: This function is guaranteed 
 * to ALWAYS return feedback to the caller,
 * even if the file does not exist.
 * 
 * @param filepath: path to the specific file to check.
 * @return webpage_t*: a reconstructed webpage object.
 * @return NULL: file does not exist.
 */
webpage_t* pagedir_load(const char* filepath);

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
int pagedir_count(const char* pageDirectory);

#endif /*__PAGEDIR_H */
