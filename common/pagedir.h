/**
 * @file pagedir.h
 * @author Amittai J. Wekesa (@siavava)
 * @brief: pagedir library: exports functionality from pagedir.c 
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef __PAGEDIR_H

#define __PAGEDIR_H

#include <stdbool.h>
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

bool pagedir_check(char* dirName);

webpage_t* pagedir_load(const char* filepath);

#endif /*__PAGEDIR_H */
