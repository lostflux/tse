/**
 * @file index.h
 * @author Amittai J. Wekesa (@siavava)
 * @brief: index data structure;
 * exports functionality of an index hashtable
 * useful to store word, document ID, count data.
 * 
 * Functionality is defined in index.c
 * 
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef __INDEX_H

#define __INDEX_H

/*********** Header Files ************/

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* Memory library */
#include "mem.h"

/* Data Structures */
#include "hashtable.h"
#include "counters.h"
#include "set.h"

/************ Function Prototypes *************/

/* opaque struct */
typedef struct index index_t;

/**
 * @function: index_new()
 * @brief: Creates a new index_t object.
 * returns a pointer to the created object.
 * 
 * Allocates memory where needed.
 * Caller must later free that memory by calling index_delete().
 * 
 * Inputs: none.
 * 
 * Returns:
 * @return index_t*: pointer to created index_t* object.
 * @return NULL: initialization failed.
 */
index_t* index_new();


/**
 * @function: index_insert()
 * @brief: Inserts word and document ID into the index.
 * Expects the index to be a  valid pointer to mallc'ed memory
 * Previously initialized by index_new().
 * If pointer to index is NULL,
 * exits without performing any actions.
 * 
 * Inputs:
 * @param index: pointer to an index object.
 * @param word: word to enter into index.
 * @param docID: document where word was found.
 * 
 * Returns: none.
 */
void index_insert(index_t* index, char* word, int docID);

/**
 * @function: index_set
 * @brief: sets the docID entry for specified word to the provided count.
 * If document ID counter for the specific word does not yet exist,
 * creates a new counter and initializes it to the specified count.
 * 
 * @param index: pointer to a valid index object.
 * @param word: word to be inserted.
 * @param docID: document ID where word was found.
 * @param count: pre-determined count of word in the document ID.
 */
void index_set(index_t* index, char* word, int docID, int count);

/**
 * @function: index_iterate()
 * @brief: iterates over items in the index,
 * calling the itemfunc on all items stored in the index.
 * 
 * Inputs:
 * @param index: pointer to an index object.
 * @param arg: pointer to an argument, e.g. a file.
 * @param itemfunc: pointer to a function.
 * 
 * Returns: none.
 */
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));

/**
 * @function: index_print()
 * @brief: prints out the data inside provided index in a specific format.
 * -> each word is printed on a new line.
 * -> for each word, the document ID and counts
 *    are printed alternating each other on the line.
 * 
 * Inputs:
 * @param index: pointer to an index object
 * @param fp: pointer to a file.
 * 
 * Returns: none.
 */
void index_print (index_t* index, FILE* fp);

/**
 * @function: index_delete()
 * @brief: Deletes an index object created using index_new().
 * No actions are taken if pointer to index item is NULL.
 * However, the itemfunc for deleting items in the index can be NULL.
 *  * 
 * Inputs:
 * @param index: pointer to an index object to be deleted.
 * 
 * Returns: none.
 */
void index_delete(index_t* index);

int** index_rank(index_t* index, char* word, char* indexFileName);


#endif /* __INDEX_H */