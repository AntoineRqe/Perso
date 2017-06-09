#ifndef DICT_H
#define DICT_H

#include "list.h"

unsigned int count_words_in_string(char* sentence, const char delimiter, List *list);
unsigned int parse_dict(char* dict_name, List* word_list);
unsigned int parse_text(char* text_name, List* word_list);
char *custom_getline(FILE *fd);

/* --------------------------------------------------------------------------------
 *                                      Tests
 * ------------------------------------------------------------------------------ */
void test_count_words_in_string(void);
void test_parse_dict(void);

#endif // DICT_H
