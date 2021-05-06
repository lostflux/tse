/**
 * @file index.h
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

/**************** Function Prototypes ********************/
typedef struct index index_t;

index_t* index_new();

void index_insert(index_t* index, char* word, int docID);

void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));

void index_delete(index_t* index);

void index_print (index_t* index, FILE* fp);