/**
 * @file index.c
 * @author Amittai J. Wekesa (@siavava)
 * @brief: index data structure;
 * defines functionality of an index hashtable
 * useful to store word, document ID, count data.
 * 
 * Functionality is exported through index.h
 * 
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
#include <ctype.h>
#include <string.h>

/* Memory library */
#include "mem.h"

/* Data Structures */
#include "hashtable.h"
#include "counters.h"
#include "set.h"

/* file handler */
#include "file.h"

/* word handler */
#include "word.h"

/* self */
#include "index.h"

/******** Static function prototypes *********/
static void printWord(void* arg, const char* key, void* item);
static void printCounts(void* arg, int key, int count);
static void deleteCounter(void* arg);
// static void rankPages(void* arg, int key, int value);



/************** Struct types **************/
typedef struct index {
  hashtable_t* ht;
} index_t;


/********** Library Functions ***********/

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
  index->ht = hashtable_new(200);
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
 * @function: index_find
 * @brief: searches for a key in the index. 
 * 
 * @param index: the index wherein to search.
 * @param word: the word to search for.
 * 
 * Returns:
 * @return: counters_t* ctrs -> counters matching word.
 * @return: NULL -> word (key) does not exist in the index. 
 */
counters_t*
index_find(index_t* index, char* word)
{
  /* make sure parameters are valid */
  assert(index != NULL && word != NULL);

  /*
   * return whatever matches the word in the hashtable.
   * if word does not exist, hashtable_find returns NULL.
   */
  return hashtable_find(index->ht, word);
}


/**
 * @function: index_set
 * @brief: see index.h for full documentation.
 * 
 * @param index: pointer to a valid index object.
 * @param word: word to be inserted.
 * @param docID: document ID where word was found.
 * @param count: pre-determined count of word in the document ID.
 */
void
index_set(index_t* index, char* word, int docID, int count)
{
  assert(index != NULL && word != NULL);

  counters_t* ctrs;
  if ((ctrs = hashtable_find(index->ht, word)) != NULL) {
    counters_set(ctrs, docID, count);
  }
  else {
    ctrs = counters_new();
    counters_set(ctrs, docID, count);
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
 * @function: index_load
 * @brief: See index.h for full documentation.
 * 
 * Inputs:
 * @param pageDirectory: page directory to search for saved webpages
 * 
 * Outputs:
 * @return index_t*: a pointer to the created index struct
 * @return NULL: Error reading from file, creating struct, or some other error occurred.
 */
index_t*
index_load(const char* indexFileName)
{

  /*
   * if error opening file, return NULL
   */
  FILE* indexFile;
  if ( (indexFile = fopen(indexFileName, "r")) == NULL) {
    fprintf(stderr, "Error accessing index file: '%s'\n", indexFileName);
    return NULL;
  }
  /* log progress */
  // logProgress(0, "output", output);

  /*
   * If error initializing index, return NULL
   */
  index_t* index;
  if ( (index = index_new()) == NULL) {
    fclose(indexFile);
    return NULL;
  }

  char* rawText = NULL;       // track text found from source file
  char* foundWord = NULL;     // track actual words that are found
  int docID; int count;       // track document ID and counts that are found 
  int pos = 0;                // track position ([x]'th word) in current sentence

  while ( (rawText = file_readWord(indexFile) ) != NULL) {
    
    /* if word has alphabetical characters */
    if (isalpha(rawText[0]) != 0) {

      /* free previously found word */
      if (foundWord != NULL) {
        mem_free(foundWord);
      }

      //reset position to zero
      pos = 0;

      // normalize the word
      normalizeWord(rawText);

      // allocate word, copy text found
      foundWord = mem_calloc_assert(strlen(rawText), 2*sizeof(char), "Error allocating memory for found word.");

      // copy found word
      strcpy(foundWord, rawText);

      /* log progress */
      // logProgress(2, "word", foundWord);

      // increment position in sentence.
      pos++;
    }
    else if (pos % 2 == 1) {
      
      // parse document ID
      docID = atoi(rawText);

      // increment position in sentence.
      pos++;
    }
    /* if count value, insert pair into index */
    else {
      
      // parse count
      count = atoi(rawText);

      // insert word, document ID, count into the index
      index_set(index, foundWord, docID, count);

      // increment position in the sentence
      pos++;
    }

    /* free the raw text */
    mem_free(rawText);
  }

  /*
   * at end of document (if read was successful), 
   * free last found valid word 
   */
  if (foundWord != NULL) {
    mem_free(foundWord);
  }

  // close the file
  fclose(indexFile);

  /* return re-created index */
  return index;

} /* end of index_load() */




/********** Static functions definitions ************/

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

  FILE* fp;
  if ( (fp = (FILE*) arg) != NULL) {

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

// /**
//  * @function: rankPages
//  * @brief: static helper function to compare and rank keys in a counters object
//  * 
//  * @param arg: an array maintaining counts
//  * @param key: the key in the counter
//  * @param value: the value in the counter
//  * @param NUMOAGES: The number of pages in the directory.
//  */
// static void
// rankPages(void* arg, int key, int value)
// {
//   assert(arg != NULL);

//   int** ranks = (int**) arg;
//   int* keys = ranks[0];
//   int* values = ranks[1];

//   assert(keys != NULL && values != NULL);

//   const int NUMPAGES = keys[0]; 

//   /* check value against current saved scores */
//   for (int i=1; i<=NUMPAGES; i++) {

//     /*
//      * if it ranks better than a current value,
//      * shift all values after the value by 1, all keys by 1
//      * and save the key-value pair into the respective arrays.
//      */
//     if (value > values[i]) {
//       for (int j=NUMPAGES; j>i; j--) {
//         keys[i] = keys[i-1];
//         values[i] = values[i-1];
//       }
//       keys[i] = key;
//       values[i] = value;
//     }
//   }
// }

// int**
// index_rank(index_t* index, char* word, char* indexFileName)
// {
//   /* make sure parameters are valid */
//   assert(index != NULL && word != NULL);


//   FILE* indexFile;
//   if ( (indexFile = fopen(indexFileName, "r")) == NULL) {
//     fprintf(stderr, "Cannot find index file.");
//     return NULL;
//   }
//   // count the number of lines in the file
//   int NUMLINES = file_numLines(indexFile);

//   // close the file
//   fclose(indexFile);


//   /* check if word exists, get counters */
//   counters_t* ctrs;
//   if ( (ctrs = index_find(index, word)) != NULL) {
//     // create array of ranks
//     int** ranks = mem_malloc_assert(2*NUMLINES*sizeof(int), "Error allocatinf ranks array in index_rank");

//     /* initialize the array */
//     ranks[0][0] = NUMLINES;
//     ranks[1][0] = -1;

//     for (int i=1; i<=NUMLINES; i++) {
//       ranks[0][i] = 0;
//       ranks[1][i] = 0;
//     }
//     /* build the ranks */
//     counters_iterate(ctrs, ranks, rankPages);

//     /* return array of ranks */
//     return ranks;
//   }
//   else {
//     return NULL;
//   }
// }