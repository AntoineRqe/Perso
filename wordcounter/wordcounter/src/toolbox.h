#ifndef DICT_H
#define DICT_H

#include <stdio.h>

#include "list.h"

/**
 * \fn int count_words_in_string(char* sentence, const char delimiter, List *list);
 * \brief count number of words in a string, spaced with the delimiter, and
 * \param sentence path to file to read.
 * \param delimiter created list of strinfs from dictionnary.
 * \param words created list of strings from dictionnary.
 *
 * \return number of words, -1 if failure.
 */
int count_words_in_string(const char * sentence, const char delimiter, List * words);

/**
 * \fn int parse_dict(char * path_to_file, List* words)
 * \brief read file in path as a dictonnary, and create a list of words from that dictionnary
 * \param path_to_file path to file to read.
 * \param words created list of strings from dictionnary.
 *
 * \return number of words, -1 if failure.
 */
int parse_dict(const char * path_to_file, List * words);

/**
 * \fn int parse_text(const char* path_to_file, List* words);
 * \brief read file in path as a text, and create a list of words from that text.
 * \param path_to_file path to file to read.
 * \param words created list of strings from dictionnary.
 *
 * \return number of items, -1 if failure.
 */
int parse_text(const char * path_to_file, List* words);

char *custom_getline(FILE * fd);
char* clean_raw_input_from_stdin(char * raw_input);


#endif // DICT_H
