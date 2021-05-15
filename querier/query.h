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

typedef struct query query_t;

query_t* query_new();

void query_delete(query_t* query);

query_t* query_build(index_t* index, char** words);

void query_intersection(index_t* index, query_t* query, char* nextWord);

query_t* query_union(query_t* subQuery1, query_t* subQuery2);

void query_index(query_t* query, char* pageDirectory);

void query_savedir(query_t* query, char* pageDirectory);

void query_print(query_t* query, FILE* fp);

counters_t* query_getCounters(query_t* query);
