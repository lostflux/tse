# CS50 Tiny Search Engine (TSE) utility library

These modules support the TSE functionality.  [pagedir](pagedir.h) offers utility functions for TSE file IO activity, [word](word.h) offers a utility function shared by the indexer and querier to normalize words extracted from webpages or from the commandline, and [index](index.h) offers a utility abstraction of a webpage index, built on top of a [hashtable](../libcs50/hashtable.h), a [set](../libcs50/set.h), and a [counter](../libcs50/counters.h).

## Usage

To build `common.a`, run `make`.

To clean up, run `make clean`.

## Overview

* [index](index.h) - an abstraction of a search engine index with utility functions.

```c
index_t* index_new();
void index_insert(index_t* index, char* word, int docID);
void index_set(index_t* index, char* word, int docID, int count);
void index_iterate(index_t* index, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));
void index_print (index_t* index, FILE* fp);
void index_delete(index_t* index);
```

* [pagedir](pagedir.h) - utility library for reading from and writing into files.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_check(char* dirName);
webpage_t* pagedir_load(const char* filepath);
```

* [word](word.h) - a utility library for processing words.

```c
void normalizeWord(char* word);
```
