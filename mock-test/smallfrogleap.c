#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))
#define DEBUG (1)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)

void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");
}


int solution(int X, int A[], int N) {
	int i = 0, index = 1;
    int count[100001];
    memset(count, 0, 100001 * sizeof(*count));

	for(i = 0; i < N; i++)
	{
		count[A[i]]++;
		while(count[index] != 0)
		{
			index++;
			if(index > X)
				return i;
		}
	}

	return -1;
}


typedef struct test_s{
	int input_data;
	int input_tab[10];
	int size;
	int output;
}test_t;

int main(void)
{
	int i = 0;

	test_t tests[] = 
	{
		{.input_data=1, .input_tab={1}, .size = 1, .output=0},
		{.input_data=2, .input_tab={1}, .size = 1, .output=-1},
		{.input_data=1, .input_tab={2}, .size = 1, .output=-1},
		{.input_data=2, .input_tab={1,2}, .size = 2, .output=1},
		{.input_data=5, .input_tab={1,3,1, 4, 2, 3, 5, 4}, .size = 8, .output=6},
		{.input_data=5, .input_tab={1,3,1, 4, 6, 3, 5, 4}, .size = 8, .output=-1}
	};

	for(i = 0; i < COUNT_ITEMS(tests); i++)
	{
		if(solution(tests[i].input_data, tests[i].input_tab, tests[i].size) != tests[i].output)
		{
			PRINT_DEBUG("test|%d] failed", i);
			exit(-1);
		}
	}

	return 0;
}
