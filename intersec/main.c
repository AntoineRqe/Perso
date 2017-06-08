#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "dict.h"

#define TEST_MODE 0

void all_tests(void);

int main(int argc, char **argv){
    #if TEST_MODE
    all_tests();
    #endif

    int i = 0;
    unsigned int nb_dict_word = 0;
    unsigned int nb_text_word = 0;
    List* dict_list = initialisation();

    if(argc <= 1 || strlen(argv[1]) <= 0){
        printf("At least one argument to give\n");
        exit(EXIT_FAILURE);
    }

    nb_dict_word = parse_dict(argv[1], dict_list);
    printf("There are %d words in the dictionnary %s\n", nb_dict_word, argv[1]);

    if(argc >= 3){
        for(i = 2; i < argc; i++){
			List* text_list = initialisation();
            nb_text_word = parse_text(argv[i], text_list);
            printf("There are %d words in text %s\n", nb_text_word, argv[i]);
            compare_list(dict_list->head, text_list->head);
            liberate(text_list);
        }
    }
	liberate(dict_list);
    return 0;
}

/* All code related to testing */

void all_tests(void){
    test_count_words_in_string();
    test_chained_list();
    test_parse_dict();
}
