#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

/* Functions and structures related to chained list */

List *initialisation(void){
    List *list = (List*)malloc(sizeof(List));
    Element *elt = (Element*)malloc(sizeof(Element));

    if(list == NULL || elt == NULL){
        exit(EXIT_FAILURE);
    }

    elt->word = "";
    elt->next = NULL;
    list->head = elt;
    return list;
}

void insert(List* list, char* word_to_add, int size){
    Element *new = (Element*)malloc(sizeof(Element));

    if(list == NULL || new == NULL){
        exit(EXIT_FAILURE);
    }

    char* new_word = (char*)malloc(sizeof(char) * size);
    snprintf(new_word, size, word_to_add);

    new->word = new_word;
    new->next = list->head;
    list->head = new;
}

unsigned int count(Element* start){
    if (start == NULL){
        return 0;
    }
    return 1 + count(start->next);
}

void print_list(Element* node){
    while(node != NULL){
        printf("'%s' -> ", node->word);
        node = node->next;
    }
    printf("NULL\n");
}

Element* search_word(Element *node, char* word){
    Element* tmp = node;
    while(tmp != NULL){
        if(!strncmp(tmp->word, word, strlen(word)+1)){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

int word_occurences(Element* node, char* word){
    int counter = 0;
    if(word == NULL || node == NULL){
        return 0;
    }

    while((node = search_word(node, word)) != NULL){
        node = node->next;
        counter++;
    }
    return counter;
}

void liberate(List * list){
    while(list->head != NULL){
        Element *tmp = list->head;
        list->head = list->head->next;
        free(tmp->word);
        free(tmp);
        liberate(list);
    }
}

void compare_list(Element* dict_list, Element* text_list){
    int occurence = 0;
    while(dict_list != NULL){
        if(strlen(dict_list->word) == 0){
            dict_list = dict_list->next;
            continue;
        }
        occurence = word_occurences(text_list, dict_list->word);
        if(occurence > 0){
            printf("%d \t %s\n", occurence, dict_list->word);
        }
        dict_list = dict_list->next;
    }
    printf("\n");
}


/* --------------------------------------------------------------------------------
 *                                      Tests
 * ------------------------------------------------------------------------------ */

void test_chained_list(void){
    List* top = initialisation();
    if(top->head == NULL || strcmp(top->head->word,"") || top->head->next != NULL){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    insert(top, "coucou!", strlen("coucou!")+1);
    insert(top, "coucou!", strlen("coucou!")+1);
    insert(top, "coucou!", strlen("coucou!")+1);
    insert(top, "coucou!", strlen("coucou!")+1);
    insert(top, "bonjour!", strlen("bonjour!")+1);
    insert(top, "aurevoir!", strlen("aurevoir!")+1);

    if(count(top->head) != 7){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    if (word_occurences(top->head, "coucou!") != 4){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    if (word_occurences(top->head, "bonjour!") != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    if (word_occurences(top->head, "aurevoir!") != 1){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    if (word_occurences(top->head, "couc!") != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    liberate(top);
    if(count(top->head) != 0){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }
}
