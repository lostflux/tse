/**
 * @file query.c
 * @author Amittai J.Wekesa (@siavava)
 * @brief: This file contains definition and functionality 
 * of a struct to handle back-end functionality for the TSE querier
 * @version 0.1
 * @date 2021-05-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* webpage handler */
#include "webpage.h"

/* data types */
#include "index.h"
#include "counters.h"

/* file handler */
#include "file.h"

/* helper library */
#include "pagedir.h"

/*     self     */
#include "query.h"


/*********** Static Function Prototypes *********/
static void dubCounters(void* arg, int docID, int count);
static void intersect(void* arg, int docID, int count);
static void unite(void* arg, int docID, int count);
static void sort(void* arg, int docID, int count);
static void setZero(void* arg, int docID, int count);


/* global constants */
static const int MAXPAGES = 200;


typedef struct query {
  int numWords;
  int numPages;
  int* docIDs;
  char* pageDirectory;
  webpage_t** pages;
  counters_t* ctrs;
} query_t;





/*
 * getter method
 */
counters_t*
query_getCounters(query_t* query)
{
  return query->ctrs;
}




/**
 * @brief Constructor
 * 
 * @return query_t*: a pointer to a  query_t object
 */
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

  /* initialize struct pointers to NULL */
  query->docIDs = NULL;
  query->pageDirectory = NULL;
  query->pages = NULL;

  /* initialize counters */
  query->ctrs = counters_new();

  /* return the pointer to the created query struct */
  return query;
}




/**
 * @brief deconstructor
 * 
 * @param query: pointer to a valid query_t object
 */
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
      for (int i=0; ; i++) {
        if (query->pages[i] == NULL) {
          break;
        }
        webpage_delete(query->pages[i]);
      }
      mem_free(query->pages);
    }
    mem_free(query);
  }
}




/**
 * @brief: assembles the results of a specific word's occurrence in the index.
 * Returns NULL in case of error or non-existence.
 * Also frees the passed in word. 
 * 
 * @param index: pointer to valid index object.
 * @param subQuery: the sequence of words to be checked in the index.
 * @return query_t*: a query struct containing the results.
 */
query_t*
query_build(index_t* index, char** subQuery)
{
  if (index != NULL && subQuery != NULL) {
    /* create query struct */
    query_t* query = query_new();

    if (query != NULL) {
      int i = 0;                                    // track index in words
      char* word = NULL;                            // track current word
      while( (word = subQuery[i]) != NULL) {

        // find the intersection with next word
        query_intersection(index, query, word);

        // free word
        mem_free(word);

        // step to next index
        i++;
      }

      /* free the subQ */
      mem_free(subQuery);

      /* return generated query struct */
      return query;
    }

    /* free the subQ */
    mem_free(subQuery);

    /* error creating query -- return NULL */
    return NULL;
  }

  if (subQuery != NULL) {
    /* free the subQ */
    mem_free(subQuery);
  }

  /* index is NULL or subQ is NULL; return NULL */
  return NULL;  
}




/**
 * @brief: calculates the intersection of two queries in the index.
 * 
 * @param index: a pointer to a valid index_t* object.
 * @param query: pointer to a valid query_t* object
 * @param nextWord: next word to check and intersect with current results in the query.
 */
void 
query_intersection(index_t* index, query_t* query, char* nextWord) {

  /* confirm all parameters are valid */
  if ( (index != NULL) && (query != NULL) && (nextWord != NULL) ) {

    /* find counters of word */
    counters_t* nextCounts = index_find(index, nextWord);

    /* 
     * if word does not exist in page (NULL return),
     * set all counts in query to zero
     */
    if (nextCounts == NULL) {
      /* set all in query to 0 */
      counters_iterate(query->ctrs, query->ctrs, setZero);
      query->numWords++;
    }

    /* 
     * if query is empty (initial step)
     * copy the counters of first word into query.
     */
    else if (query->numWords == 0) {

      /* copy the counters */
      counters_iterate(nextCounts, query->ctrs, dubCounters);

      /* increment the number of words in query */
      query->numWords++;
    }
    else {

      counters_t** buffer = mem_malloc(sizeof(query->ctrs) + sizeof(nextCounts));
      counters_t* intersection = counters_new();
      buffer[0] = nextCounts;
      buffer[1] = intersection;

      counters_iterate(query->ctrs, buffer, intersect);
      
      counters_delete(query->ctrs);
      query->ctrs = intersection;

      mem_free(buffer);

      query->numWords++;
    }
  }
}




/**
 * @brief: function to build a union of the results of two subqueries.
 * This function also deletes the input subqueries after computing their union.
 * 
 * @param subQuery1: pointer to a query struct.
 * @param subQuery2: pointer to a query struct.
 * @return query_t*: a query struct whose results constitute a union of the input subqueries.
 */
query_t*
query_union(query_t* subQuery1, query_t* subQuery2)
{
  /* assert all parameters are valid */
  if (subQuery1 == NULL || subQuery2 == NULL) {
    return NULL;
  }

  /* 
   * create query struct for the union
   * on ERROR, return NULL to the caller.
   */
  query_t* query;
  if ( (query = query_new()) == NULL) {
    /* delete the two original sub queries */
    fprintf(stderr, "Error creating query for union.");
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

  /* delete the two original sub queries */
  query_delete(subQuery1);
  query_delete(subQuery2);

  /* return unioned query */
  return query;
}




/**
 * @brief: merge the query results with data from the index and pages.
 * 
 * @param query: pointer to a query struct.
 * @param pageDirectory: file directory wherein pages are saved.
 */
void
query_index(query_t* query, char* pageDirectory)
{
  if (query == NULL || query->ctrs == NULL) {
    return;
  }

  /* buffer to hold sorted docID-value pairs */
  int** buffer = mem_calloc_assert(2, MAXPAGES*sizeof(int), "Error allocating memory in query_index.");

  buffer[0] = mem_calloc(MAXPAGES, sizeof(int));
  buffer[1] = mem_calloc(MAXPAGES, sizeof(int));

  char* sampleURL = mem_calloc(50, sizeof(char));
  strcpy(sampleURL, "sampleURL.com");
  webpage_t* samplePage = webpage_new(sampleURL, MAXPAGES, NULL);

  query->pages = mem_calloc_assert(MAXPAGES, sizeof(samplePage), "Error allocating memory in query_index");

  webpage_delete(samplePage);

  query->docIDs = mem_calloc_assert(MAXPAGES, sizeof(int), "Error allocating memory in query_index.");

  /* iterate through counters to sort calues. */
  if (query->ctrs != NULL) {
    counters_iterate(query->ctrs, buffer, sort);
  }

  /* create counters of sorted values */
  counters_t* sorted = counters_new();
  int* keys = buffer[0];
  int* counts = buffer[1];

  /* index pages */
  for (int i=0; ; i++) {

    /* at ZERO key (end of items), save it and break the loop. */
    if (keys[i] == 0) {
      query->numPages = i;
      break;
    }

    /* load current page and save it to query */
    char filepath[strlen(pageDirectory) + 10];
    if (pageDirectory[strlen(pageDirectory)-1] == '/') {
      sprintf(filepath, "%s%d", pageDirectory, keys[i]);
    }
    else {
      sprintf(filepath, "%s/%d", pageDirectory, keys[i]);
    }
    webpage_t* page = pagedir_load(filepath);
    
    // query->pages = NULL;
    query->pages[i] = page;
    query->docIDs[i] = keys[i];
  }

  /* add in values in the new order */
  for (int i=0; i<query->numPages; i++) {
    counters_set(sorted, keys[i], counts[i]);
  }

  /* delete old counters */
  counters_delete(query->ctrs);

  /* save new counters */
  query->ctrs = sorted;

  /* free the buffer */
  mem_free(keys);
  mem_free(counts);
  mem_free(buffer);
}




/**
 * @brief: prints the results held in a query struct.
 * 
 * @param query: pointer to a valid query struct
 * @param fp: pointer to a file open for wtiring (could be stdout).
 */
void
query_print(query_t* query, FILE* fp)
{
  /* make sure query and file pointers are valid */
  if (query != NULL && fp != NULL) {

    /* get array of pages, docIDs */
    webpage_t** pages = query->pages;
    int* docIDs = query->docIDs;
    if (pages != NULL && docIDs != NULL) {

      /* if no pages matched, print no pages */
      if (query->numPages == 0) {
        fprintf(fp, "Matches %d documents.\n", query->numPages);
      }

      /* else, print the number of pages found
       and each page's number of matches, 
       document ID, URL */
      else{
        fprintf(fp, "Matches %d documents (ranked):\n", query->numPages);
        for (int i = 0; i<query->numPages; i++) {
          int docID = docIDs[i];
          if (docID > 0) {
            int score = counters_get(query->ctrs, docID);
            char* url = webpage_getURL(pages[i]);
            fprintf(fp, "score %3d doc %3d: %s\n", score, docID, url);
          }
        }
      }
    }
  }
}




/**
 * @brief: sorts a duo of arrays containing key-value pairs
 * 
 * @param arg: pointer to two coupled arrays.
 * @param docID: document ID (key)
 * @param count: count
 */
static void
sort(void* arg, int docID, int count)
{
  if (arg != NULL) {

    /* convert back to 2D array */
    int** buffer = (int**) arg;
    int* keys = buffer[0];
    int* counts = buffer[1];

    /* find size of array */
    int size = 0;
    for (int i=0; ; i++) {
      if (counts[i] == 0) {
        break;
      }
      size = i+1;
    }

    /* loop through array to find point where values fit in. */
    for (int i=0; i<=size+1; i++) {
      if (count > counts[i]) {

        /* shift values after index by 1 to the right. */
        for (int j=size+1; j>i; j--) {
          keys[j] = keys[j-1];
          counts[j] = counts[j-1];
        }

        /* save in the values */
        keys[i] = docID;
        counts[i] = count;
        break;
      }
    }
  }
}




/**
 * @brief: create a union of two counters sets.
 * receives key-value pairs from one counters set
 * and compounds them into the other counters set.
 * 
 * @param arg: pointer to a counters struct
 * @param docID: document ID (key)
 * @param count: count
 */
static void
unite(void* arg, int docID, int count)
{
  /* check all parameters are valid */
  if (arg != NULL) {
    counters_t* ctrs = (counters_t*) arg;

    /* get current count of docID in counters */
    int currentCount = counters_get(ctrs, docID);

    /* save max to counters */
    int total = currentCount + count;
    counters_set(ctrs, docID, total);
  }
}




/**
 * @brief: create a copy of a counters:
 * Receives key-value pairs and saves them in the counter.
 * 
 * @param arg: pointer to a counters
 * @param docID: document ID (key)
 * @param count: count
 */
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




/**
 * @brief: reset values in counters to zero
 * 
 * @param arg: pointer to a valid counters struct
 * @param docID: document ID (key)
 * @param count: count
 */
static void
setZero(void* arg, int docID, int count)
{
  if(arg != NULL) {
    /* convert back to counters_t* */
    counters_t* ctrs = (counters_t*) arg;

    /* insert docID-count pair into the copy */
    counters_set(ctrs, docID, 0);
  }
}




/**
 * @brief: builds an intersection of two counts.
 * Compares occurence of keys and values from one counters
 * with occurrences in the other. 
 * 
 * Inputs:
 * @param arg: array of two counters
 * @param docID: document ID (key)
 * @param count: count
 */
static void
intersect(void* arg, int docID, int count) {
  if(arg != NULL) {
    /* convert back to counters_t* */
    counters_t** buffer = (counters_t**) arg;

    counters_t* next = buffer[0];
    counters_t* intersection = buffer[1];

    int nextCount = counters_get(next, docID);

    if (nextCount > 0) {
      /* find minimum value of the two */
      int min = (nextCount < count) ? nextCount : count;

      /* save to intersection */
      counters_set(intersection, docID, min);
    }
  }
}
