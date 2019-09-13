#ifndef ASCII_H
#define ASCII_H

#define ALPHABET_COUNT	(4)
#define ALPHABET_X_SIZE (5)
#define ALPHABET_Y_SIZE (7)
#define SPACE_SIZE (1)

#define GET_ASCII_OFFSET(letter) (letter - 65)

typedef struct chained_string_s
{
	int							index;
	struct chained_string_s*	next;

} chained_string_t;

static const char alphabet_ascii[ALPHABET_COUNT][ALPHABET_Y_SIZE][ALPHABET_X_SIZE] =
{
	{
		{0, 1, 1, 1, 0},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 1, 1, 1, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1}
	},
	{
		{1, 1, 1, 1, 0} ,
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 1, 1, 1, 0},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 1, 1, 1, 0}
	},
	{
		{0, 1, 1, 1, 0} ,
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 0},
		{1, 0, 0, 0, 0},
		{1, 0, 0, 0, 0},
		{1, 0, 0, 0, 1},
		{0, 1, 1, 1, 0}
	},
	{
		{1, 1, 1, 1, 0} ,
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 1, 1, 1, 0}
	}
};

int string_to_list(char* text, chained_string_t** head);
void erase_list(chained_string_t* head);

#endif // !ASCII_H