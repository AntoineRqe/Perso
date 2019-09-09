#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ascii.h"

int string_to_list(char* text, clist_t* head)
{
	int i = 0, offset = 0;
	clist_t* last = head;

	while (text != '\0')
	{
		if (text[i] == ' ')
		{
			i++;
			continue;
		}
		else if (!isalpha(text[i]))
		{
			printf("%c is not alphanumeric", text[i]);
			goto failure;
		}

		offset = GET_ASCII_OFFSET(toupper(text[i]));

		if (offset < 0 || offset >= ALPHABET_COUNT)
		{
			printf("%c is not alphanumeric", text[i]);
			goto failure;
		}

		if (!head)
		{
			clist_t* tmp_list = (clist_t*)calloc(1, sizeof(clist_t));
			tmp_list->letter = alphabet_ascii[offset][0];
			tmp_list->next = NULL;
			last = tmp_list;
			head = tmp_list;
		}
		else
		{
			clist_t* tmp_list = (clist_t*)calloc(1, sizeof(clist_t));
			tmp_list->letter = alphabet_ascii[offset][0];
			tmp_list->next = NULL;
			last->next = tmp_list;
			last = tmp_list;
		}

		i++;
	}

	return 0;

failure:
	erase_list(head);
	return -1;
}

void erase_list(clist_t* head)
{
	clist_t* cur = head;
	clist_t* next = head;

	while(next)
	{
		next = cur->next;
		free(cur);
		cur = next;
	} 
}