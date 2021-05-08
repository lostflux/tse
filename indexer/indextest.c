/**
 * @file indextest.c
 * @author Amittai J. Wekesa (@siavava)
 * @brief: A unit test for the index data structure
 * defined in ../common/index.c
 * and exported in ../common/index.h 
 * @version 0.1
 * @date 2021-05-07
 * 
 * @copyright Copyright (c) 2021
 */

/******** Header Files *********/

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* mem library */
#include "mem.h"

/* file library */
#include "file.h"

/* word library */
#include "word.h"

/* index data structure */
#include "index.h"


/******** Function Prototypes *********/
static void logProgress(const int depth, const char* operation, const char* item);

int 
main(const int argc, const char* argv[])
{

  /* if incorrect usage, exit. */
  if (argc != 3) {
    fprintf(stderr, "Invalid usage.\n");
    fprintf(stderr, "Usage: ./indextest [SOURCE FILE] [OUTPUT FILE]\n");
    exit(1);
  }

  /* parse text for source file */
  char source[strlen(argv[1])];
  strcpy(source, argv[1]);

  /* these were useful for quick tests. */
  // char* source = "../data/output/wikipedia-1.index";
  // char* output = "../data/output/wikipedia-1-regenerated.index";

  /* open source file */
  FILE* sourceFile;
  if ( (sourceFile = fopen(source, "r")) == NULL) {
    fprintf(stderr, "Error opening source file: '%s'\n", source);
    exit(2);
  }
  logProgress(0, "source", source);


  /* parse text for output file */
  char output[strlen(argv[2])];
  strcpy(output, argv[2]);

  /* open output file */
  FILE* outputFile;
  if ( (outputFile = fopen(output, "w")) == NULL) {
    fprintf(stderr, "Error opening output file: '%s'\n", output);

    /* close input file opened earlier */
    fclose(sourceFile);
    exit(3);
  }
  /* log progress */
  logProgress(0, "output", output);

  /* create index object */
  index_t* index = index_new();

  char* rawText = NULL;       // track text found from source file
  char* foundWord = NULL;     // track actual words that are found
  int docID; int count;       // track document ID and counts that are found 
  int pos = 0;                // track position ([x]'th word) in current sentence

  while ( (rawText = file_readWord(sourceFile) ) != NULL) {
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
    else {
      
      // parse count
      count = atoi(rawText);

      // insert word, document ID, count into the index
      index_set(index, foundWord, docID, count);

      /* log progress */
      
      // char* insertion = mem_calloc(strlen(foundWord)+10, 2*sizeof(char));
      // sprintf(insertion, "%s; %d, %d", foundWord, docID, count);
      // logProgress(4, "inserted", insertion);
      // mem_free(insertion);

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

  /* print index to output file */
  index_print(index, outputFile);

  /* delete the index */
  index_delete(index);

  fclose(sourceFile);
  fclose(outputFile);

  return 0;
}

/* Function to log progress */
static void 
logProgress(const int depth, const char* operation, const char* item)
{
/*
 * Checks for a flag and logs progress of program to stdout.
 * Different cases for start/continuing/end.
 */
#ifdef LOGPROGRESS
  if (strcmp(operation, "START") == 0) {
    printf("%s %*s%7s\n", item, 2*depth, "  ", operation);
  }
  else if (strcmp(operation, "END") == 0) {
    printf("%*s%7s %s\n", 2*depth, "  ", operation, item);
  }
  else {
    printf("%*s%7s: %s\n", 2*depth, "  ", operation, item);
  }
#else
  ;
#endif
}