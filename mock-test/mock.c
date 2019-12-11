#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define DEBUG (0)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)
#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))

static void print_tab(int * tab, size_t len)
{
	size_t i = 0;
	for(i = 0; i < len; i++)
		printf("%d ", *(tab + i));
	printf(" Size : %lu\n", len);
}

int solution(int A[], int N, int K) {
	uint32_t top_suite = 0;
	int i = 0;

	print_tab(A, N);

	for(i = 0; i < N; i++)
	{
		uint32_t count = 0;
		uint32_t remaining = K;
		do
		{
			int j;
			PRINT_DEBUG("%s", "------------------------");
			for(j = i; j < N; j++)
			{
				PRINT_DEBUG("Comparing A[%d]=%d vs A[%d]=%d", j, A[j], i, A[i]);
				if(A[j] != A[i])
				{
					if(remaining > 0)
					{
						remaining--;
						count++;
						PRINT_DEBUG("Remaining : %d", remaining);
					}
					else
						break;
				}
				else
					count++;

				PRINT_DEBUG("Count : %d", count);
			}

			if(remaining > 0)
			{
				PRINT_DEBUG("Check still remaining : %d", remaining);
				if(i == 0);
				else
				{
					for(j = (i - 1); j >= 0; j--)
					{
						PRINT_DEBUG("Comparing A[%d]=%d vs A[%d]=%d", j, A[j], i, A[i]);
						if(A[j] != A[i])
						{
							if(remaining > 0)
							{
								remaining--;
								count++;
								PRINT_DEBUG("Remaining : %d", remaining);
							}
							else
								break;
						}
						PRINT_DEBUG("Count : %d", count);
					}
				}
			}
			remaining = 0;
		} while(remaining > 0);

		if(count > top_suite)
			top_suite = count;
		else if(top_suite == N)
			return top_suite;
	}

	return top_suite;
}

int main(void)
{
	int test[] = {3, 1, 1, 4, 3, 3, 4};
	int test1[] = {4,5,5,4,2,2,4};
	int test2[] = {1,3,3,2};
	int test3[] = {1, 1, 3, 4, 3, 3, 4};
	int test4[] = {1, 1};
	int test5[400];

	assert(solution(test, COUNT_ITEMS(test), 2) == 4);
	assert(solution(test1, COUNT_ITEMS(test1), 0) == 2);
	assert(solution(test3, COUNT_ITEMS(test3), 2) == 5);
	assert(solution(test2, COUNT_ITEMS(test2), 2) == 4);
	assert(solution(test4, COUNT_ITEMS(test4), 0) == COUNT_ITEMS(test4));
	assert(solution(test4, COUNT_ITEMS(test4), COUNT_ITEMS(test4)) == COUNT_ITEMS(test4));
	solution(test5, COUNT_ITEMS(test5), COUNT_ITEMS(test5));

	return 0;
}
