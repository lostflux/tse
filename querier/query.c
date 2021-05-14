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
#include <assert.h>

#include "webpage.h"

#include "index.h"
#include "counters.h"

#include "query.h"


/*********** Function Prototypes *********/
static void dubCounters(void* arg, int docID, int count);
static void intersect(void* arg, int docID, int count);
static void unite(void* arg, int docID, int count);



typedef struct query {
  int numWords;
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

  /* initialize numwords to ZERO */
  query->numWords = 0;

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

    /* set to minimum value in the two */
    int min = (currentCount < count) ? currentCount : count;
    counters_set(intersection, docID, min);
  }
}