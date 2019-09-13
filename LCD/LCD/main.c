#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "custom_print.h"
#include "ascii.h"
#include "matrix.h"

#define	TEST_ITERATION (112)

int main(int argc, char* argv[])
{
	int					test = 0;
	int					i = 0;
	int					ret				= -1;
	int					time_sec		= 0;
	char				input_time[5]	= {0};
	char				input_text[128] = {0};
	char				error_buf[256]  = {0};
	chained_string_t*	head = NULL;
	matrix_t			matrix;

	if (argc == 2)
		if (!strcmp(argv[1], "test"))
			test = 1;

	if (test)
	{
		static char* text = "hello world";
		string_to_list(text, &head);

		for (i = 0; i < TEST_ITERATION; i++)
		{
			build_matrix(i, head, matrix);
			dispay_matrix(matrix);
		}
	}
	else
	{
		PRINT_INFO("desired time on lcd? (max:9999)");
		if (!(fgets(input_time, sizeof(input_time), stdin)))
		{
			PRINT_ERROR("error reading time...");
			goto failure;
		}

		if (sscanf_s(input_time, "%d", &time_sec) == EOF)
		{
			strerror_s(error_buf, sizeof(error_buf), errno);
			PRINT_ERROR("%s not a valid time : %s", input_time, error_buf);
			goto failure;
		}

		PRINT_INFO("%s", "Desired text on LCD? (max:128 letters)");

		if (!(fgets(input_text, sizeof(input_text), stdin)))
		{
			PRINT_ERROR("Error reading text...");
			goto failure;
		}

		input_text[strlen(input_text) - 1] = '\0';

		PRINT_DEBUG("Received \"%s\" for time %d.", input_text, time_sec);

		if (string_to_list(input_text, &head) < 0)
			goto failure;

		chained_string_t* cur = head;

		while (cur)
		{
			PRINT_DEBUG("%d letter is index %d", i, cur->index);
			cur = cur->next;
			i++;
		}

		if (build_matrix(time_sec, head, matrix) < 0)
			goto failure;

		dispay_matrix(matrix);
	}

	ret = 0;

failure:
	erase_list(head);
	return ret;
}