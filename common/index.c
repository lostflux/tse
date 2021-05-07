/**
 * @file index.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

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

/* self */
#include "index.h"

/******** Static functions *********/
static void printWord(void* arg, const char* key, void* item);
static void printCounts(void* arg, int key, int count);
static void deleteCounter(void* arg);



/************** Struct types **************/
typedef struct index {
  hashtable_t* ht;
} index_t;


/********** Function Definitions ***********/

/**
 * @function: index_new()
 * @brief: see index.h for documentation.
 * 
 * Inputs: none.
 * 
 * Returns:
 * @return index_t*: pointer to created index_t* object.
 * @return NULL: initialization failed.
 */
index_t* 
index_new()
{
  // allocate memory for the index object.
  index_t* index = mem_malloc_assert(sizeof(index_t), "mem alloc for index failed.");

  // initialize the hashtable to store items in the index.
  index->ht = hashtable_new(20);
  assert(index->ht != NULL);

  // return pointer to the index.
  return index;
}

/**
 * @function: index_insert()
 * @brief: see index.h for full documentation. 
 * 
 * Inputs:
 * @param index: pointer to an index object.
 * @param word: word to enter into index.
 * @param docID: document where word was found.
 * 
 * Returns: none.
 */
void
index_insert(index_t* index, char* word, int docID)
{
  assert(index != NULL);
  /*
   * if counters already exist for the word,
   * get and update it.
   * 
   * ELSE, create new counters for the word, 
   * update it with current instance, 
   * and insert into the index.
   */ 
  counters_t* ctrs;
  if ((ctrs = hashtable_find(index->ht, word)) != NULL) {
    counters_add(ctrs, docID);
  }
  else {
    ctrs = counters_new();
    counters_add(ctrs, docID);
    hashtable_insert(index->ht, word, ctrs);
  }
}

/**
 * @function: index_iterate()
 * @brief: see index.h for full documentation.
 * 
 * Inputs:
 * @param index: pointer to an index object.
 * @param arg: pointer to an argument, e.g. a file.
 * @param itemfunc: pointer to a function.
 * 
 * Returns: none.
 */
void 
index_iterate(index_t* index, void* arg, 
void (*itemfunc)(void* arg, const char* key, void* item))
{
  assert(index != NULL);
  
  /*
   * Get hashtable in index, 
   * if hashtable is NULL do nothing.
   */
  hashtable_t* ht = index->ht;
  if (ht != NULL) {

    // call hashtable_iterate() over the hashtable.
    hashtable_iterate(ht, arg, itemfunc);
  }
}

/**
 * @function: deleteCounter()
 * @brief: static helper function to call counters_delete()
 * 
 * @param arg: pointer to an object, expected to be of type counters_t
 */
static void 
deleteCounter(void* arg)
{
  assert(arg != NULL);
  /*
   * call counters_delete() for the counters object
   * after converting it to appropriate type.
   */
  counters_t* ctrs = (counters_t*) arg;
  counters_delete(ctrs);
}

/**
 * @function: index_delete()
 * @brief: see index.h for full documentation.
 * 
 * Inputs:
 * @param index: pointer to an index object to be deleted.
 * 
 * Returns: none.
 */
void 
index_delete(index_t* index)
{
  assert(index != NULL);
  /*
   * call hashtable_delete() for the hashtable in the index,
   * with the deleter for counters.
   */
  hashtable_delete(index->ht, deleteCounter);

  // free the memory allocated to the index object.
  mem_free(index);
}

/**
 * @function: printCounts()
 * @brief: static helper function to print counters_t objects.
 * 
 * Inputs:
 * @param arg: pointer to an object expected to be file. 
 * @param key: key from counter
 * @param count: value associated to key in counter.
 */
static void
printCounts(void* arg, int key, int count)
{
  assert(arg != NULL);

  FILE* fp = (FILE*) arg;
  if (fp != NULL) {

    // print the key -- count pairs to file. 
    fprintf(fp, " %d %d", key, count);
  }
}

/**
 * @function: printWord
 * @brief: static helper function to print words
 * and call printCounts for their respective counters.
 * 
 * Inputs:
 * @param arg: pointer to an object, expected to be a file.
 * @param key: key in set -- word
 * @param item: pointer to object associated to key; expected type counters-t
 * 
 * Returns: none.
 */
static void
printWord(void* arg, const char* key, void* item)
{
  assert(arg != NULL);

  FILE* fp;
  if ((fp = (FILE*) arg) != NULL) {
    // print the key (word) to file
    fprintf(fp, "%s", key);

    /*
     * iterate over the counters with the file
     * and the function to print counts.
     */
    counters_t* ctrs = (counters_t*) item;
    counters_iterate(ctrs, fp, printCounts);

    // print a new line.
    fprintf(fp, "\n");
  }
}


/**
 * @function: index_print()
 * @brief: see index.h for full documentation.
 * 
 * Inputs:
 * @param index: pointer to an index object
 * @param fp: pointer to a file.
 * 
 * Returns: none.
 */
void 
index_print (index_t* index, FILE* fp)
{
  assert(index != NULL);
  assert(fp != NULL);

  // iterate over the index object with printWord
  index_iterate(index, fp, printWord);
}
