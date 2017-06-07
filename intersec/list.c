#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void insert(List* list, char* word_to_add){
    Element *new = (Element*)malloc(sizeof(Element));

    if(list == NULL || new == NULL){
        exit(EXIT_FAILURE);
    }

    new->word = word_to_add;
    new->next = list->head;
    list->head = new;
}

int count(Element* start){
    if (start == NULL){
        return 0;
    }
    return 1 + count(start->next);
}

void print_list(Element* start){
    if(start != NULL){
        printf("'%s' -> ", start->word);
        print_list(start->next);
    }
}

void test_chained_list(void){
    List* top = initialisation();
    if(top->head == NULL || strcmp(top->head->word,"") || top->head->next != NULL){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    insert(top, "coucou!");
    insert(top, "bonjour!");
    insert(top, "aurevoir!");

    if(count(top->head) != 4){
        printf("[%s][%d] KO\n", __FUNCTION__, __LINE__);
    } else {
        printf("[%s][%d] OK\n", __FUNCTION__, __LINE__);
    }

    print_list(top->head);
}
