#pragma once

#define ALPHABET_COUNT	(2)
#define ALPHABET_X_SIZE (5)
#define ALPHABET_Y_SIZE (7)

#define GET_ASCII_OFFSET(letter) (letter - 65)

typedef struct c_list_s
{
	const char* letter;
	void* next;

}clist_t;

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
	}

};

int string_to_list(char* text, clist_t* head);
void erase_list(clist_t* head);