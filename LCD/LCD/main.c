#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "ascii.h"

#define MATRIX_X_SIZE (64)
#define MATRIX_Y_SIZE (8)

int main(int argc, char* argv[])
{
	int			ret				= -1;
	int			time_sec		= 0;
	char		input_time[5]	= {0};
	char		input_text[128] = {0};
	char		error_buf[256]  = {0};
	char		matrix[MATRIX_X_SIZE][MATRIX_Y_SIZE] = {0};
	clist_t*	head = NULL;

	memset(matrix, 0, MATRIX_X_SIZE * MATRIX_Y_SIZE);

	printf("Desired time on LCD? (max:9999)\n");
	if (!(fgets(input_time, sizeof(input_time), stdin)))
	{
		printf("Error reading time...");
		goto failure;
	}

	if (sscanf_s(input_time, "%d", &time_sec) == EOF)
	{
		strerror_s(error_buf, sizeof(error_buf), errno);
		printf("%s not a valid time : %s", input_time, error_buf);
		goto failure;
	}

	printf("Desired text on LCD? (max:128 letters)\n");
	if (!(fgets(input_text, sizeof(input_text), stdin)))
	{
		printf("Error reading text...");
		goto failure;
	}

	input_text[strlen(input_text) - 1] = '\0';

	printf("Received %s for time %d.", input_text, time_sec);

	if (string_to_list(input_text, head) < 0)
		goto failure;

	ret = 0;

failure:
	erase_list(head);
	return ret;
}