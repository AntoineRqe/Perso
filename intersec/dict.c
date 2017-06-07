#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "dict.h"

/* Return the number of words in a given sentence
 * and store them in a given chained list*/

static void send_word_to_list(char* word, int size, List *list){
	char* word_in_text = (char*)malloc(sizeof(char) * (size + 1));
	snprintf(word_in_text, size, word);
	insert(list, word_in_text, size);
	free(word_in_text);
}

unsigned int count_words_in_string(char* sentence, const char delimiter, List *list){
    unsigned int words_counter = 0;
    char* current_ptr = sentence;
    char* old_ptr = sentence;
    unsigned int word_size = 0;
	
    if(current_ptr == NULL || strlen(sentence) == 0){
        return 0;
    }

    char* end_ptr = sentence + strlen(sentence);
    if (strlen(sentence) <= 1 && *current_ptr != delimiter &&
    (('A' <= *current_ptr && *current_ptr <= 'Z') || ('a' <= *current_ptr && *current_ptr <= 'z'))) {
		send_word_to_list(current_ptr, 2, list);
		return 1;
    }

    while((current_ptr = strchr(current_ptr, delimiter)) != NULL){
        if((current_ptr-old_ptr) <= 1){ //Two consecutive delimiter
            current_ptr++;
            old_ptr = current_ptr;
            continue;
        }
        if(!strncmp(old_ptr, " ", 1)){
			old_ptr++;
		}

        word_size = current_ptr - old_ptr + 1;

        if(word_size >= MAX_WORD_SIZE){
            continue;
        }
        send_word_to_list(old_ptr, word_size, list);

        words_counter++;
        old_ptr = current_ptr;
        current_ptr++;
    }

    word_size = end_ptr - old_ptr + 1;
    if(word_size > 2 && word_size < MAX_WORD_SIZE){
        if(!strncmp(old_ptr, " ", 1)){
			old_ptr++;
		}
		
		send_word_to_list(old_ptr, word_size, list);
        words_counter++;
    }
    return words_counter;
}

/* Read the file of the dictionnary, put name in list and return number of value; */
unsigned int parse_dict(char* dict_name, List* word_list){
    FILE* fd = NULL;
    char line[MAX_WORD_SIZE] = "";
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
            insert(word_list, line, strlen(line));
        }
    }

    fclose(fd);
    return count(word_list->head) - 1;
}

/* --------------------------------------------------------------------------------
 *                                      Tests
 * ------------------------------------------------------------------------------ */
char* test_sentences[] = {
    "je m appelle antoine",     // 4 words
    "Je m appelle Antoine.",    // 4 words
    " ",                        // 0 word
    " Antoine ",                // 1 word
    "",                         // 0 word
    "jemappelleantoine",        // 1 word
    "a",                        // 1 word
    "! an be",                  // 2 words
    NULL                        // 0 word
};

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
    unsigned int total_counter = 0;
    List* test_list = initialisation();

    test_counter = count_words_in_string(test_sentences[0], ' ', test_list);
    if(test_counter != 4){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[1], ' ', test_list);
    if(test_counter != 4){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[2], ' ', test_list);
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[3], ' ', test_list);
    if(test_counter != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[4], ' ', test_list);
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[5], ' ', test_list);
    if(test_counter != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[6], ' ', test_list);
    if(test_counter != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[7], ' ', test_list);
    if(test_counter != 2){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    total_counter += test_counter;

    test_counter = count_words_in_string(test_sentences[8], ' ', test_list);
    if(test_counter != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    
    total_counter += test_counter;

    if(total_counter != (count(test_list->head) - 1)){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
    print_list(test_list->head);
}
