#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "custom_print.h"
#include "ascii.h"

#define MATRIX_X_SIZE (64)
#define MATRIX_Y_SIZE (8)

int main(int argc, char* argv[])
{
	int			i = 0;
	int			ret				= -1;
	int			time_sec		= 0;
	char		input_time[5]	= {0};
	char		input_text[128] = {0};
	char		error_buf[256]  = {0};
	char		matrix[MATRIX_X_SIZE][MATRIX_Y_SIZE] = {0};
	chained_string_t*	head = NULL;

	memset(matrix, 0, MATRIX_X_SIZE * MATRIX_Y_SIZE);

	/*printf("desired time on lcd? (max:9999)\n");
	if (!(fgets(input_time, sizeof(input_time), stdin)))
	{
		printf("error reading time...");
		goto failure;
	}*/

	//if (sscanf_s(input_time, "%d", &time_sec) == eof)
	//{
	//	strerror_s(error_buf, sizeof(error_buf), errno);
	//	printf("%s not a valid time : %s", input_time, error_buf);
	//	goto failure;
	//}

	PRINT_INFO("%s", "Desired text on LCD? (max:128 letters)");

	if (!(fgets(input_text, sizeof(input_text), stdin)))
	{
		printf("Error reading text...");
		goto failure;
	}

	input_text[strlen(input_text) - 1] = '\0';

	PRINT_INFO("Received \"%s\" for time %d.", input_text, time_sec);

	if (string_to_list(input_text, &head) < 0)
		goto failure;

	chained_string_t* cur = head;

	while (cur)
	{
		PRINT_INFO("%d letter is index %d", i, cur->index);
		cur = cur->next;
		i++;
	}

	ret = 0;

failure:
	erase_list(head);
	return ret;
}