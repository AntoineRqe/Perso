#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "custom_print.h"
#include "ascii.h"

int string_to_list(char* text, chained_string_t** head)
{
	int i = 0, offset = 0;
	chained_string_t* tmp_list = NULL;
	chained_string_t* tmp_head = NULL;
	chained_string_t* last = NULL;

	while (text[i] != '\0')
	{
		if (text[i] == ' ')
		{
			i++;
			continue;
		}
		else if (!isalpha(text[i]))
		{
			PRINT_ERROR("\'%c\' is not alphanumeric", text[i]);
			goto failure;
		}

		offset = GET_ASCII_OFFSET(toupper(text[i]));

		if (offset < 0 || offset >= ALPHABET_COUNT)
		{
			PRINT_ERROR("\'%c\' is not in alphabet", text[i]);
			goto failure;
		}

		if (!(tmp_list = (chained_string_t*)malloc(sizeof(chained_string_t))))
		{
			PRINT_ERROR("impossible to allocate memory");
			goto failure;
		}

		tmp_list->index = offset;
		tmp_list->next = NULL;

		if (!tmp_head)
		{
			last = tmp_list;
			tmp_head = tmp_list;
		}
		else
		{
			last->next = tmp_list;
			last = tmp_list;
		}

		i++;
	}

	*head = tmp_head;

	return 0;

failure:
	erase_list(tmp_head);
	return -1;
}

void erase_list(chained_string_t* head)
{
	chained_string_t* cur = head;
	chained_string_t* next = head;

	while(next)
	{
		next = cur->next;
		free(cur);
		cur = next;
	} 
}