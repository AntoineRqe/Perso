#include <string.h>
#include "matrix.h"

static int _get_first_display_char(int offset, int * pos, int * macro_pos)
{
	int diff = MATRIX_X_SIZE - offset;

	// Inside the matrix
	if (diff >= 0)
	{
		*pos = 0;
		*macro_pos = 0;
	}
	//Partially out of the matrix
	else if (diff >= -5)
	{
		*pos = 0;
		*macro_pos = -diff;
	}
	// Totally out the matrix
	else
	{
		*pos = -(diff / (ALPHABET_X_SIZE + 1));
		* macro_pos = (-diff) % (ALPHABET_X_SIZE + 1);
	}

	return 0;
}

static void _reset_matrix(matrix_t matrix)
{
	int y = 0;

	for (y = 0; y < MATRIX_Y_SIZE; y++)
		memset(matrix[y], 0, MATRIX_X_SIZE);
}

int build_matrix(int offset, chained_string_t* head, matrix_t matrix)
{
	int first = 1;
	int pos = (MATRIX_X_SIZE - 1);
	int init_pos = 0, init_macro_pos = 0;
	int x = 0, y = 0;

	//All '0' if offset is 0.
	if (offset <= 0)
	{
		_reset_matrix(matrix);
		return 0;
	}

	// Start matrix at offset
	_get_first_display_char(offset, &init_pos, &init_macro_pos);

	PRINT_DEBUG("Init pos : %d, Init macro pos : %d", init_pos, init_macro_pos);

	// Get the first letter to display
	while (init_pos <= 0)
	{
		if (!head)
		{
			PRINT_DEBUG("All text overflowed, nothing to print");
			_reset_matrix(matrix);
			return 0;
		}

		head = head->next;
		init_pos--;
	}

	while (head)
	{
		// Draw the first letter into the matrix
		if (first)
		{
			for (y = 0; y < MATRIX_Y_SIZE; y++)
				memcpy(&matrix[y][pos], &alphabet_ascii[head->index][y][init_macro_pos], (ALPHABET_X_SIZE - init_macro_pos));

			pos -= init_macro_pos + 1;
			first = 0;
		}
		else
		{
			for (y = 0; y < MATRIX_Y_SIZE; y++)
				memcpy(&matrix[y][pos], &alphabet_ascii[head->index][y][pos], ALPHABET_X_SIZE);

			pos -= (ALPHABET_X_SIZE + 1);
			
		}

		head = head->next;
	}

	return 0;
}

void dispay_matrix(matrix_t matrix)
{
	int x = 0, y = 0;

	printf("%.*s\n", MATRIX_X_SIZE, "*******************************************************************************************************");

	for (y = 0; y < MATRIX_Y_SIZE; y++)
	{
		for (x = 0; x < MATRIX_X_SIZE; x++)
			printf("%d", matrix[y][x]);
		printf("\n");
	}

	printf("%.*s\n", MATRIX_X_SIZE, "*******************************************************************************************************");
}