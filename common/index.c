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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mem.h"
#include "hashtable.h"
#include "counters.h"
#include "set.h"
#include "index.h"

/**************** Function Prototypes ********************/
index_t* index_new();
void index_insert(index_t* index, char* word, int docID);
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));
void index_delete(index_t* index);

// Functions to print indexes
void index_print (index_t* index, FILE* fp);
static void printWord(FILE* fp, char* key, void* item);
static void printCounts(FILE* fp, char* key, int count);



/************** Struct types *******************/

typedef struct index {
  hashtable_t* ht;
} index_t;


/*************** Function Definitions ***************/
index_t* 
index_new()
{
  index_t* index = mem_malloc_assert(sizeof(index_t), "mem alloc for index failed.");
  index->ht = hashtable_new(20);
}

void
index_insert(index_t* index, char* word, int docID)
{
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

void 
index_iterate(index_t* index, void* arg, 
void (*itemfunc)(void* arg, const char* key, void* item))
{
  // if NULL index, do nothing
  if (index != NULL) {

    // Get hashtable in index, if NULL do nothing
    hashtable_t* ht = index->ht;
    if (ht != NULL) {
      hashtable_iterate(ht, arg, itemfunc);
    }
  }
}

void 
index_delete(index_t* index)
{
  hashtable_delete(index->ht, counters_delete);
  mem_free(index);
}

static void
printCounts(FILE* fp, char* key, int count)
{
  if (fp != NULL) {
    fprintf(fp, " %d %d", key, count);
  }
}

static void
printWord(FILE* fp, char* key, void* item)
{
  fprintf(fp, key);
  counters_t* ctrs = (counters_t*) item;
  counters_iterate(ctrs, fp, printCounts);
  fprintf(fp, "\n");
}


void 
index_print (index_t* index, FILE* fp)
{
  index_iterate(index, fp, printWord);
}
