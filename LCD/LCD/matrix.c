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
	else if (diff > -5)
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
	int x = 0, y = 0;

	for (y = 0; y < MATRIX_Y_SIZE; y++)
		for(x = 0; x < MATRIX_X_SIZE; x++)
			matrix[y][x] = 0;
}

int build_matrix(int time, chained_string_t* head, matrix_t matrix)
{
	chained_string_t* cur = head;
	int first = 1;
	int pos = -1;
	int init_pos = 0, init_macro_pos = 0;
	int x = 0, y = 0;

	_reset_matrix(matrix);
	//All '0' if offset is 0.
	
	if (time <= 0)
		return 0;

	// Start matrix at offset
	_get_first_display_char(time, &init_pos, &init_macro_pos);

	PRINT_DEBUG("Init pos : %d, Init macro pos : %d", init_pos, init_macro_pos);

	// 1 seconde -> offset = 0
	pos = (time - 1);

	// Get the first letter to display
	if ((init_pos > 0) || ((init_pos == 0) && (init_macro_pos > 0)))
	{
		while (init_pos > 0)
		{
			if (!cur)
			{
				PRINT_DEBUG("All text overflowed, nothing to print");
				return 0;
			}

			cur = cur->next;
			init_pos--;
		}
		pos = (MATRIX_X_SIZE - 1);
	}

	while (cur)
	{
		int tmp_offset = 0;
		// Draw the first letter into the matrix
		if (first)
		{
			for (y = 0; y < ALPHABET_Y_SIZE; y++)
			{
				for (x = 0; x < (ALPHABET_X_SIZE - init_macro_pos); x++)
				{
					tmp_offset = pos - x;
					if (tmp_offset < 0)
						break;
					matrix[(y + 1)][tmp_offset] = alphabet_ascii[cur->index][y][x + init_macro_pos];
				}
			}

			pos -= ((ALPHABET_X_SIZE - init_macro_pos) + SPACE_SIZE);
			first = 0;
		}
		else
		{
			for (y = 0; y < ALPHABET_Y_SIZE; y++)
				for (x = 0; x < ALPHABET_X_SIZE; x++)
				{
					tmp_offset = pos - x;
					if (tmp_offset < 0)
						break;
					matrix[(y + 1)][tmp_offset] = alphabet_ascii[cur->index][y][x];
				}

			pos -= (ALPHABET_X_SIZE + SPACE_SIZE);
		}

		cur = cur->next;
	}

	return 0;
}

void dispay_matrix(matrix_t matrix)
{
	int x = 0, y = 0;

	printf("%.*s\n", MATRIX_X_SIZE, "======================================================================================================");

	for (y = 0; y < MATRIX_Y_SIZE; y++)
	{
		for (x = (MATRIX_X_SIZE-1); x >= 0; x--)
			if(matrix[y][x])
				printf("*");
			else
				printf(" ");
		printf("\n");
	}

	printf("%.*s\n", MATRIX_X_SIZE, "======================================================================================================");
}