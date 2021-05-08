/**
 * @file word.h
 * @author Amittai J. Wekesa (@siavava)
 * @brief 
 * @version 0.2
 * @date 2021-05-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/******** Header Files *********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/******* Function Prototypes ******/

/**
 * @function: normalizeWord
 * @brief: normalizes the provided word 
 * by converting all characters in the word to lower-case.
 * No action is done if pointer to word is NULL.
 * 
 * Inputs:
 * @param word: a word (char pointer) to be normalized.
 * 
 * Returns: none.
 */
void normalizeWord(char* word);