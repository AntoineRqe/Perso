#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))
#define DEBUG (1)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)

int arithmetic_sum(int first_item, int last_item)
{
	if(last_item == first_item)
		return first_item;

	return last_item + arithmetic_sum(first_item, (last_item -1));
}

void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");
}

int sum_tab(int * A, int N)
{
	if(N == 1)
		return A[0];

	return A[0] + sum_tab(&A[1], (N-1));
}

int solution(int A[], int N)
{
	int i = 0;
	int total_sum = 0;
	int left_sum = 0;
	int right_sum = 0;
	int min = 0x0FFFFFFF;

	total_sum = sum_tab(A, N);
	PRINT_DEBUG("%s", "--------------------");
	PRINT_DEBUG("total sum : %d", total_sum);

	for(i = 0; i < (N - 1); i++)
	{
		int diff = 0;
		left_sum += A[i];
		right_sum = total_sum - left_sum;
		diff = fabs(left_sum - right_sum);
		PRINT_DEBUG("[P=%d] left : %d - right %d = %d", i, left_sum, right_sum, diff);
		if(diff < min)
			min = diff;
	}

	PRINT_DEBUG("min %d", min);
	return min;
}

typedef struct test_s{
	int input[];
	int output;
}test_t;

int main(void)
{
	int i = 0;

	test_t tests[] = 
	{
		{.input={3,1,2,4,3}, .output=1},
		{.input={-1000,1000}, .output=2000},
		{.input={0, 0}, .output=0},
		{.input={0,-1}, .output=1}
	}

	for(i = 0; i < COUNT_ITEMS(tests); i++)
	{
		if(solution(tests[i].input, COUNT_ITEMS(tests[i].input)
	}

	assert(solution(test, COUNT_ITEMS(test)) == 1);
	assert(solution(test2, COUNT_ITEMS(test2)) == 2000);
	assert(solution(test3, COUNT_ITEMS(test3)) == 0);
	assert(solution(test4, COUNT_ITEMS(test4)) == 1);

	return 0;
}
