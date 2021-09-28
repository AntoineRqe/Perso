#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

/* Functions and structures related to chained list */

List* init(void)
{
    List *list = (List*)malloc(sizeof(List));

    if(!list)
        return NULL;

    list->head = NULL;

    return list;
}

void fini(List * list)
{
    if(!list)
        return;

    Element * current = list->head;

    while(current)
    {
        Element *tmp = current;
        current = current->next;

        if(tmp->word)
            free(tmp->word);

        if(tmp)
            free(tmp);
    }

    free(list);
}

int insert(List * list, const char * new_word)
{
    int ret = -1;

    if(!new_word)
        goto fail;
    else if(new_word[0] == '\0')
        goto fail;

    if(!list)
        goto fail;

    Element * new = (Element*)malloc(sizeof(Element));

    if(!new)
        goto fail;

    asprintf(&(new->word), "%s", new_word);

    // Case of 1st insert
    if(!(list->head))
    {
        new->next = NULL;
        list->head = new;
    }
    else
    {
        new->next = list->head;
        list->head = new;
    }

    ret = 0;

fail:
    return ret;
}

unsigned int count_items(Element* head)
{
    if(!head)
        return 0;
    else if(!(head->word))
        return 0;

    return 1 + count_items(head->next);
}

Element* _search_word(Element *head, const char* word)
{
    Element* tmp = head;

    while(tmp)
    {
        if(!strncmp(tmp->word, word, strlen(word)+1))
            return tmp;

        tmp = tmp->next;
    }

    return NULL;
}

int count_occurences(Element* head, const char* word)
{
    int counter = 0;

    if(!(word && head))
        return -1;
    else if(!(head->word))
        return 0;

    while((head = _search_word(head, word)))
    {
        head = head->next;
        counter++;
    }

    return counter;
}

void compare_list(Element* dictionnary, Element* text)
{
    int occurence = 0;

    while(dictionnary)
    {
        if(strlen(dictionnary->word) == 0)
        {
            dictionnary = dictionnary->next;
            continue;
        }

        occurence = count_occurences(text, dictionnary->word);

        if(occurence > 0)
            printf("%d \t %s\n", occurence, dictionnary->word);

        dictionnary = dictionnary->next;
    }

}
