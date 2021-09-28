#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toolbox.h"

int main(int argc, char **argv){

    int i = 0;
    int nb_dict_word = 0;
    int nb_text_word = 0;
    List* dictionnary = init();

    if(argc <= 1 || strlen(argv[1]) <= 0){
        printf("At least one argument to give\n");
        exit(EXIT_FAILURE);
    }

    nb_dict_word = parse_dict(argv[1], dictionnary);
    printf("There are %d words in the dictionnary %s\n\n", nb_dict_word, argv[1]);

    if (argc == 2)
    {
        List* stdin_list = init();
        char* stdin_string = NULL;

        printf("Enter input> ");
        fflush(stdout);

        stdin_string = custom_getline(stdin);
        stdin_string = clean_raw_input_from_stdin(stdin_string);
        nb_text_word = count_words_in_string(stdin_string, ' ', stdin_list);

        compare_list(dictionnary->head, stdin_list->head);

        printf("\n");
        printf("%d \t total words\n", nb_text_word);

        free(stdin_string);
        fini(stdin_list);

    }
    else if(argc >= 3)
    {
        for(i = 2; i < argc; i++)
        {
            List* text = init();

            nb_text_word = parse_text(argv[i], text);

            printf("parsing file %s\n\n", argv[i]);

            compare_list(dictionnary->head, text->head);

            printf("\n");
            printf("%d \t total words\n\n", nb_text_word);

            fini(text);
        }
    }

    fini(dictionnary);

    return 0;
}
