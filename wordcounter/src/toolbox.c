#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "toolbox.h"

/* Return the number of words in a given sentence
 * and store them in a given chained list*/
static int _send_word_to_list(const char* word, int size, List *list)
{
    int ret = -1;

    char* new = (char*)malloc(sizeof(char) * (size + 1));

    if(!new)
        goto end;

    snprintf(new, size, "%s", word);

    ret = insert(list, new);        

end:
    free(new);
    return ret;
}

/* Own implementation of getline to read line from file descriptor without size limitation */
char *custom_getline(FILE *fd)
{
    size_t size = 0;
    size_t len  = 0;
    size_t last = 0;
    char *buf = NULL;

    if(feof(fd))
        goto fail;

    do
    {
        size += BUFSIZ;
        buf = realloc(buf, size);

        if(!buf)
            goto fail;

        fgets(buf + last, size, fd);

        len = strlen(buf);
        last = len - 1;

    } while (!feof(fd) && buf[last] != '\n');

    // Remove newline from buffer
    buf[last] = '\0';

    return buf;

fail:
    free(buf);
    return NULL;
}

/* Remplace EOL by space */
char* clean_raw_input_from_stdin(char *raw_input)
{
    if(raw_input == NULL)
        return NULL;

    char* fini_string = (char*)malloc(sizeof(char) * (strlen(raw_input) + 1));
    snprintf(fini_string, strlen(raw_input) + 1, "%s", raw_input);
    char *tmp = fini_string;

    while((tmp = strchr(tmp, '\n')) != NULL){
        *tmp = ' ';
    }
    return fini_string;
}

/* Count number of word in a string, word are separated by a delimiter */
int count_words_in_string(const char * sentence, const char delimiter, List * words)
{
    unsigned int words_counter = 0;
    const char* current_ptr = sentence;
    const char* old_ptr = sentence;
    unsigned int word_size = 0;

    if(current_ptr == NULL || sentence[0] == '\0')
        return 0;

    const char* end_ptr = sentence + strlen(sentence);

    if (strlen(sentence) <= 1 && *current_ptr != delimiter &&
    (('A' <= *current_ptr && *current_ptr <= 'Z') || ('a' <= *current_ptr && *current_ptr <= 'z'))) {
        _send_word_to_list(current_ptr, 2, words);
        return 1;
    }

    while((current_ptr = strchr(current_ptr, delimiter)) != NULL)
    {
        if((current_ptr-old_ptr) <= 1)
        {
            current_ptr++;
            old_ptr = current_ptr;
            continue;
        }
        if(!strncmp(old_ptr, " ", 1))
        {
            old_ptr++;
        }

        word_size = current_ptr - old_ptr + 1;

        _send_word_to_list(old_ptr, word_size, words);

        words_counter++;
        old_ptr = current_ptr;
        current_ptr++;
    }

    word_size = end_ptr - old_ptr + 1;

    if(word_size > 2){
        if(!strncmp(old_ptr, " ", 1)){
            old_ptr++;
        }

        _send_word_to_list(old_ptr, word_size, words);
        words_counter++;
    }

    return words_counter;
}

/* Read the file text_name, store word in a give list and returns the total word counter */
int parse_text(const char* path_to_file, List* words)
{
    int ret = -1;
    FILE* fd = NULL;
    char *line = NULL;
    int total_counter = 0;

    if(!(path_to_file && words))
        goto end;

    fd = fopen(path_to_file, "r");

    if(!fd)
    {
        printf("Error opening file %s : %s\n", path_to_file, strerror(errno));
        goto end;
    }

    while((line = custom_getline(fd)) != NULL)
    {
        total_counter += count_words_in_string(line, ' ', words);
        free(line);
    }

    ret = total_counter;

end:
    if(fd)
        fclose(fd);

    return ret;
}

int parse_dict(const char * path_to_file, List* words)
{
    int ret = -1;

    FILE* fd = NULL;
    char *line = NULL;
    unsigned int read = 0;

    if(!(path_to_file && words))
        goto end;

    fd = fopen(path_to_file, "r");

    if(!fd)
    {
        printf("Error opening file %s : %s\n", path_to_file, strerror(errno));
        goto end;
    }

    while((line = custom_getline(fd)) != NULL)
    {
        unsigned int i = 0;
        read = strlen(line);

        if(read <= 1)
        {
            free(line);
            continue;
        }

        for(i = 0; i < read; i++)
        {
            if(!(('A' <= line[i] && line[i] <= 'Z') ||
                ('a' <= line[i] && line[i] <= 'z')))
            {
                    break;
            }
        }

        // Add in list if only [a-Z] in the string
        if(i == read)
        {
            if(insert(words, line) < 0)
            {
                free(line);
                goto end;
            }
        }

        free(line);
    }

    ret = count_items(words->head);

end:
    if(fd)
        fclose(fd);
    return ret;
}
