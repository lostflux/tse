/**
 * @file word.c
 * @author Amittai J. Wekesa (@siavava)
 * @brief: a (short) library to process and normalize
 * words extracted from websites.
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 */

/******** Header Files *********/

/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* self */
#include "word.h"


/**
 * @function: normalizeWord
 * @brief: see word.h for documentation.
 * 
 * Inputs:
 * @param word: a word (char pointer) to be normalized.
 * 
 * Returns: none.
 */
void
normalizeWord(char* word)
{
  /* make sure pointer to word is valid */
  assert(word != NULL);                     

   // step through length of word.
  for (int i = 0; i < strlen(word); i++) {

    // normalize each char in word.
    word[i] = tolower(word[i]);
  }
}
