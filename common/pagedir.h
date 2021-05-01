/**
 * @file pagedir.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-04-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */


void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
bool pagedir_init(const char* pageDirectory);
