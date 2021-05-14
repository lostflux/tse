/**
 * @file query.c
 * @author Amittai J.Wekesa (@siavava)
 * @brief 
 * @version 0.1
 * @date 2021-05-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "webpage.h"

#include "index.h"
#include "counters.h"

#include "query.h"

#include "pagedir.h"

#include "file.h"


/*********** Function Prototypes *********/
static void dubCounters(void* arg, int docID, int count);
static void intersect(void* arg, int docID, int count);
static void unite(void* arg, int docID, int count);
static void sort(void* arg, int docID, int count);



typedef struct query {
  int numWords;
  int numPages;
  int* docIDs;
  char* pageDirectory;
  webpage_t** pages;
  counters_t* ctrs;
} query_t;

query_t*
query_new()
{
  /* allocate memory for query */
  query_t* query = mem_malloc(sizeof(query_t));

  /* if error allocating space, return NULL */
  if (query == NULL) {
    return NULL;
  }

  /* initialize numwords, numpages to ZERO */
  query->numWords = 0;
  query->numPages = 0;

  /* initialize pageDirectory to NULL */
  query->pageDirectory = NULL;

  /* initialize counters */
  query->ctrs = counters_new();

  /* return the pointer to the created query struct */
  return query;
}

void 
query_delete(query_t* query)
{
  if (query != NULL) {
    if (query->ctrs != NULL) {
      counters_delete(query->ctrs);
    }
    if (query->docIDs != NULL) {
      mem_free(query->docIDs);
    }
    if (query->pages != NULL) {
      mem_free(query->pages);
    }
    mem_free(query);
  }
}

query_t*
query_build(index_t* index, char** words)
{
  assert(index != NULL && words != NULL);

  /* create query struct */
  query_t* query = query_new();

  int i = 0;                                    // track index in words
  char* word = NULL;                            // track current word
  while( (word = words[i]) != NULL) {

    // find the intersection
    query_intersection(index, query, word);

    // increment index
    i++;
  }

  /* return generated query struct */
  return query;
}

void 
query_intersection(index_t* index, query_t* query, char* nextWord) {

  /* confirm all parameters are valid */
  if ( (index != NULL) && (query != NULL) && (nextWord != NULL) ) {

    /* find counters of word */
    counters_t* nextCounts = index_find(index, nextWord);

    /* 
     * if word does not exist in page (NULL return),
     * set counters in query to NULL.
     */
    if (nextCounts == NULL) {
      counters_delete(query->ctrs);
      query->numWords++;
    }

    /* 
     * if query is empty (initial step)
     * copy the counters of first word into query.
     */
    else if (query->numWords == 0) {

      /* find counters of word */
      counters_t* nextCounts = index_find(index, nextWord);

      /* copy the counters */
      counters_iterate(nextCounts, query->ctrs, dubCounters);

      /* increment the number of words in query */
      query->numWords++;
    }
    else {
      /* find the counters of next word */
      counters_t* nextCounts = index_find(index, nextWord);
      if (nextCounts != NULL) {

        /* create counters of intersection */
        counters_t* intersection = counters_new();

        assert(intersection != NULL);

        /* copy counts inside query to intersection */
        counters_iterate(query->ctrs, intersection, dubCounters);

        /* iterate over counters of next word, checking for commonality */
        counters_iterate(nextCounts, intersection, intersect);

        /* delete current counters in query */
        counters_delete(query->ctrs);

        /* save generated counters of intersections to the query */
        query->ctrs = intersection;
        query->numWords++;
      }
    }
  }
}

query_t*
query_union(query_t* subQuery1, query_t* subQuery2)
{
  /* assert all parameters are valid */
  assert( (subQuery1 != NULL) && (subQuery2 != NULL) );

  /* 
   * create query struct for the union
   * on ERROR, return NULL to the caller.
   */
  query_t* query;
  if ( (query = query_new()) == NULL) {
    /* delete the two original sub queries being */
    query_delete(subQuery1);
    query_delete(subQuery2);
    return NULL;
  }

  /* 
   * set the number of words to the num of words
   * in the two queries being joined.
   */
  query->numWords = subQuery1->numWords + subQuery2->numWords;

  /* iterate over counters in first query */
  counters_iterate(subQuery1->ctrs, query->ctrs, unite);

  /* iterate over counters in second query */
  counters_iterate(subQuery2->ctrs, query->ctrs, unite);

  /* delete the two original sub queries being */
  query_delete(subQuery1);
  query_delete(subQuery2);

  /* return unioned query */
  return query;
}

void
query_index(query_t* query, char* pageDirectory)
{
  /* buffer to hold sorted docID-value pairs */
  int** buffer = mem_calloc_assert(200, 2*sizeof(int), "Error allocating memory in query_sort.");

  query->pages = mem_malloc_assert(200, "Error allocating memory in query_sort");

  query->docIDs = mem_calloc_assert(200, sizeof(int), "Error allocating memory in query_sort.");

  /* initialize all values to ZERO */
  for (int i=0; i<100; i++) {   // assumption: max num of pages = 100
    buffer[0][i] = 0;           // to hold keys
    buffer[1][i] = 0;           // to hold values
  }

  /* iterate through counters to sort calues. */
  counters_iterate(query->ctrs, buffer, sort);

  /* create counters of sorted values */
  counters_t* sorted = counters_new();
  int* keys = buffer[0];
  int* counts = buffer[1];

  /* index pages, find last index */
  int lastIndex=0;
  for (int i=0; ; i++) {

    /* at ZERO key (end of items), break the loop. */
    if (keys[i] == 0) {
      break;
    }

    /* load current page and save it to query */
    char filepath[strlen(pageDirectory) + 10];
    if (pageDirectory[strlen(pageDirectory)-1] == '/') {
      sprintf(filepath, "%d", keys[i]);
    }
    else {
      sprintf(filepath, "/%d", keys[i]);
    }
    query->pages[i] = pagedir_load(filepath);
    query->docIDs[i] = keys[i];
    lastIndex = i;
  }

  /* save last index + 1 to be total number of pages */
  query->numPages = lastIndex + 1;

  /* add in values in the new order */
  for (int i=lastIndex; i>=0; i--) {
    counters_set(sorted, keys[i], counts[i]);
  }

  /* free the buffer */
  mem_free(buffer);

  /* delete old counters */
  counters_delete(query->ctrs);

  /* save new counters */
  query->ctrs = sorted;
}

void
query_print(query_t* query, FILE* fp)
{
  if (query != NULL && fp != NULL) {
    webpage_t** pages = query->pages;
    int* docIDs = query->docIDs;
    assert(pages != NULL && docIDs != NULL);
    fprintf(fp, "Matches %d documents (ranked):\n", query->numPages);
    for (int i = 0; i<query->numPages; i++) {
      int docID = docIDs[i];
      int score = counters_get(query->ctrs, docID);
      char* url = webpage_getURL(pages[i]);
      fprintf(fp, "score  %d doc %d: %s\n", score, docID, url);
    }
  }
}


static void
sort(void* arg, int docID, int count)
{
  if (arg != NULL) {

    /* convert back to 2D array */
    int** buffer = (int**) arg;
    int* keys = buffer[0];
    int* counts = buffer[1];

    /* find size of array */
    int size;
    for (int i=0; ; i++) {
      if (keys[i] == 0) {
        break;
      }
      size = i+1;
    }

    /* loop through array to find point where values fit in. */
    for (int i=0; i<size; i++) {
      if (count > counts[i]) {

        /* shift values after index by 1 to the right. */
        for (int j=size-1; j>i; j--) {
          keys[j] = keys[j-1];
          counts[j] = counts[j-1];
        }

        /* save in the values */
        keys[i] = docID;
        counts[i] = count;
      }
    }
  }
}

static void
unite(void* arg, int docID, int count)
{
  /* check all parameters are valid */
  if (arg != NULL) {
    counters_t* ctrs = (counters_t*) arg;

    /* get current count of docID in counters */
    int currentCount = counters_get(ctrs, docID);

    /* save sums to counters */
    int total = currentCount + count;
    counters_set(ctrs, docID, total);
  }
}

static void
dubCounters(void* arg, int docID, int count)
{
  if(arg != NULL) {
    /* convert back to counters_t* */
    counters_t* copy = (counters_t*) arg;

    /* insert docID-count pair into the copy */
    counters_set(copy, docID, count);
  }
}

static void
intersect(void* arg, int docID, int count) {
  if(arg != NULL) {
    /* convert back to counters_t* */
    counters_t* intersection = (counters_t*) arg;

    /* get value in intersection */
    int currentCount = counters_get(intersection, docID);

    /* find minimum value of the two */
    int min = (currentCount < count) ? currentCount : count;

    /* if min value is greater than zero, save it. */
    if (min > 0) {
      counters_set(intersection, docID, min);
    }
  }


}
