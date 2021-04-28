/**
 * @file: crawler.c
 * @author: Amittai J. Wekesa (@siavava)
 * @brief: File containing functionality of crawler for Tiny Search Engine
 * @version: 0.1
 * @date: 2021-04-28
 * 
 * @copyright: Copyright (c) 2021
 * 
 */

/***************** Header Files ***************/
#include <stdio.h>
#include <stdlib.h>

/*********** Function Prototypes *************/
int parseArgs(const char* argv);



int 
main(int argc, const char* argv[])
{
  /* code */
  parseArgs(argv);
  return 0;
}

int
parseArgs(const char* argv)
{
  char* seedDirectory = argv[1];
  char* pageDirectory = argv[2];
  char* maxDepth = argv[3];

}

