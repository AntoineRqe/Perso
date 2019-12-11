#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define DEBUG (0)

#define LOW_BIT(A, OFFSET) ((A >> OFFSET) & 0x00000001)

int solution(int N) {

	int i = 0;
	int start = -1, is_gap = -1;
	int top_gap = 0;

	printf("Received %d\n", N);

	for(i = 0; i < 31; i++)
	{
		int bit = LOW_BIT(N, i);
		if(bit)
		{
			printf("bit %d is %d\n", i, bit);
			if(start < 0)
				start = i;
			else if(is_gap)
			{
				int gap = i - start - 1;
				top_gap = (gap > top_gap) ? gap : top_gap;
				printf("top gap is %d\n", top_gap);
				start = i;
				is_gap = 0;
			}
			else
				start = i;
		}
		else
		{
			if(start)
				is_gap = 1;
		}
	}
	return top_gap;
}

int main(void)
{
	//~ assert(solution(9) == 2);
	//~ assert(solution(529) == 4);
	//~ assert(solution(20) == 1);
	//~ assert(solution(15) == 0);
	//~ assert(solution(32) == 0);
	//~ assert(solution(0) == 0);
	//~ assert(solution(1041) == 5);
	//~ assert(solution(2147483647) == 0);
	assert(solution(74901729) == 4);

	return 0;
}
