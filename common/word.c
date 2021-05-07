/**
 * @file word.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-05-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/************** Header Files ******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "word.h"

void
normalizeWord(char* word)
{
  if (word != NULL) {
    for (int i = 0; i < strlen(word); i++) {
      word[i] = tolower(word[i]);
    }
  }
}
