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

static int 
normalizeChar(int c)
{
  switch (isupper(c)) {
    case 0: return c;
    case 1: return tolower(c);
  };
}

void
normalizeWord(char* word)
{
  if (word != NULL) {
    for (int i = 0; i < strlen(word); i++) {
      word[i] = normalizeChar(word[i]);
    }
  }
}
