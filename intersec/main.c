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
    printf("argc : %d\n", argc);
    for(i = 0; i< argc; i++){
        printf("argv[%d] : %s\n", i, argv[i]);
    }

    unsigned int nb_dict_word = 0;

    if(argc <= 1 || strlen(argv[1]) <= 0){
        printf("At least one argument to give\n");
        exit(EXIT_FAILURE);
    }

    List* dict_list = initialisation();
    nb_dict_word = parse_dict(argv[1], dict_list);
    printf("There are %d words in the dictionnary\n", nb_dict_word);

    return 0;
}

/* All code related to testing */

void all_tests(void){
    test_count_words_in_string();
    test_chained_list();
    test_parse_dict();
}
