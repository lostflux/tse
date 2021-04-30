/**
 * @file pagedir.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/************** Header Files ****************/
#include "webpage.h"
#include "pagedir.h"


/************ Function Prototypes ***********/


bool 
pagedir_init(char* pageDir)
{
  char* crawlerConfig;
  FILE* fp;
  sprintf(crawlerConfig, "%s/.crawler", pageDir);
  if ( (fp = fopen(crawlerConfig, "w") ) != NULL) {
    fclose(fp);
    return true;
  }
  return false;
}
