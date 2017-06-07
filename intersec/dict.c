#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "dict.h"

char* test_sentences[] = {
    "je m appelle antoine",     // 4 words
    "Je m appelle Antoine.",    // 4 words
    " ",                        // 0 word
    " Antoine ",                // 1 word
    "",                         // 0 word
    "jemappelleantoine",        // 1 word
    "a",                        // 1 word
    NULL                        // 0 word
};

/* Return the number of words in a given sentence
 * and store them in a given chained list*/
unsigned int count_words_in_string(char* sentence, const char delimiter){
    unsigned int words_counter = 0;
    char* current_ptr = sentence;
    char* old_ptr = sentence;

    if(current_ptr == NULL){
        return 0;
    }

    char* end_ptr = sentence + strlen(sentence);
    if ((end_ptr - current_ptr) == 1 && *current_ptr != delimiter) {
        return 1;
    }

    while((current_ptr = strchr(current_ptr, delimiter)) != NULL){
        if((current_ptr-old_ptr) <= 1){ //Two consecutive delimiter
            old_ptr = current_ptr;
            current_ptr++;
            continue;
        }
        words_counter++;
        old_ptr = current_ptr;
        current_ptr++;
    }

    if((end_ptr-old_ptr) > 1){
        words_counter++;
    }
    //printf("There are %d words in sentence '%s'\n", words_counter, sentence);
    return words_counter;
}

/* Read the file of the dictionnary, put name in list and return number of value; */
unsigned int parse_dict(char* dict_name, List* word_list){
    FILE* fd = NULL;
    char line[256] = "";
    unsigned int read = 0;

    fd = fopen(dict_name, "r");

    if(fd == NULL){
        printf("Error opening the dictionnary, Bye!\n");
        exit(EXIT_FAILURE);
    }

    while(fgets(line,sizeof(line), fd)){
        unsigned int i = 0;
        read = strlen(line);

        if( read <= 1){
            continue;
        }

        for(i = 0; i < read; i++){
            if(!(('A' <= line[i] && line[i] <= 'Z') ||
                ('a' <= line[i] && line[i] <= 'z'))){
                    break;
            }
        }
        if (i == (strlen(line) - 1)){
            insert(word_list, line);
        }
    }

    fclose(fd);
    return count(word_list->head) - 1;
}

void test_parse_dict(void){
    List* test_list = initialisation();
    unsigned int word_nb = 0;

    word_nb = parse_dict("test_dict1.txt", test_list);

    if(word_nb != 3){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    print_list(test_list->head);
    //liberate(test_list);
}

void test_count_words_in_string(void){
    unsigned int test_counter = 0;
    test_counter = count_words_in_string(test_sentences[0], ' ');
    if(test_counter != 4){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[1], ' ');
    if(test_counter != 4){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[2], ' ');
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[3], ' ');
    if(test_counter != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[4], ' ');
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[5], ' ');
    if(test_counter != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[6], ' ');
    if(test_counter != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    test_counter = count_words_in_string(test_sentences[7], ' ');
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
}
