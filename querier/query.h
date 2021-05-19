/**
 * @file query.c
 * @author Amittai J.Wekesa (@siavava)
 * @brief: Exports functions defined in query.c
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

typedef struct query query_t;

/**
 * @brief Constructor
 * 
 * @return query_t*: a pointer to a  query_t object
 */
query_t* query_new();

/**
 * @brief deconstructor
 * 
 * @param query: pointer to a valid query_t object
 */
void query_delete(query_t* query);

/**
 * @brief: assembles the results of a specific word's occurrence in the index.
 * Returns NULL in case of error or non-existence.
 * Also frees the passed in word. 
 * 
 * @param index: pointer to valid index object.
 * @param subQuery: the sequence of words to be checked in the index.
 * @return query_t*: a query struct containing the results.
 */
query_t* query_build(index_t* index, char** words);

/**
 * @brief: calculates the intersection of two queries in the index.
 * 
 * @param index: a pointer to a valid index_t* object.
 * @param query: pointer to a valid query_t* object
 * @param nextWord: next word to check and intersect with current results in the query.
 */
void query_intersection(index_t* index, query_t* query, char* nextWord);

/**
 * @brief: function to build a union of the results of two subqueries.
 * This function also deletes the input subqueries after computing their union.
 * 
 * @param subQuery1: pointer to a query struct.
 * @param subQuery2: pointer to a query struct.
 * @return query_t*: a query struct whose results constitute a union of the input subqueries.
 */
query_t* query_union(query_t* subQuery1, query_t* subQuery2);

/**
 * @brief: merge the query results with data from the index and pages.
 * 
 * @param query: pointer to a query struct.
 * @param pageDirectory: file directory wherein pages are saved.
 */
void query_index(query_t* query, char* pageDirectory);

/**
 * @brief: prints the results held in a query struct.
 * 
 * @param query: pointer to a valid query struct
 * @param fp: pointer to a file open for wtiring (could be stdout).
 */
void query_print(query_t* query, FILE* fp);

/*
 * getter method for counters containing
 * results of given query
 */
counters_t* query_getCounters(query_t* query);
