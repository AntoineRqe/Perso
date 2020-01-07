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

static int * PrefixCount(int A[], int N)
{
	int 	i = 0;
	int *			P = (int*)malloc(sizeof(int) * N);

	for(i = 0; i < N; i++)
	{
		if(i == 0)
			P[i] = A[i];
		else
			P[i] = A[i] + P[(i-1)];
	}

	return P;
}

int PassingCars(int A[], int N)
{
	int 	i = 0;
	int 	CountPairs 	= 0;
	int *	P 			= PrefixCount(A, N);

	for(i = 0; i < N; i++)
	{
		if(A[i] == 1)
			continue;

		CountPairs += (P[(N-1)] - P[i]);
		if(CountPairs > 1000000000)
			return -1;
		PRINT_DEBUG("[%d] Found %d pairs", i, (P[(N-1)] - P[i]));
	}

	return CountPairs;
}

#define CHAR_TO_IMPACT(CHAR)	\
(CHAR == 'A') ? 1 :				\
(CHAR == 'C') ? 2 :				\
(CHAR == 'G') ? 3 :				\
(CHAR == 'T') ? 4 :				\
-1

struct Results {
  int * A;
  int M; // Length of the array
};

struct Results GenomicRangeQuery(char *S, int P[], int Q[], int M)
{
	int				i = 0,j = 0, val = 0, diff = 0, len = 0;
	struct Results	result;
	int * min = (int *)malloc(sizeof(int) * M);
	int counter[5] = { 0 };

	for(i = 0; i < M; i++)
	{
		if(P[i] == Q[i])
		{
			min[i] = CHAR_TO_IMPACT(S[P[i]]);
			continue;
		}

		for(j = P[i]; j <= Q[i]; j++)
		{
			val = CHAR_TO_IMPACT(S[j]);
			counter[val]++;
		}
	}



    result.A = min;
    result.M = M;
    return result;
}

typedef struct test_s{
	int input_data;
	int input_tab[10];
	int tab_size;
	int output;
}test_t;

int main(void)
{
	//~ unsigned int i = 0;

	//~ test_t tests[] = 
	//~ {
		//~ {.input_data=1, .input_tab={0}, .tab_size = 1, .output=0},
		//~ {.input_data=1, .input_tab={1}, .tab_size = 1, .output=0},
		//~ {.input_data=1, .input_tab={0,0}, .tab_size = 2, .output=0},
		//~ {.input_data=1, .input_tab={0,1}, .tab_size = 2, .output=1},
		//~ {.input_data=1, .input_tab={1,0}, .tab_size = 2, .output=0},
		//~ {.input_data=1, .input_tab={1,1}, .tab_size = 2, .output=0},
		//~ {.input_data=1, .input_tab={0,1,0,1,1}, .tab_size = 5, .output=5},

	//~ };

	//~ for(i = 0; i < COUNT_ITEMS(tests); i++)
	//~ {
		//~ if(PassingCars(tests[i].input_tab, tests[i].tab_size) != tests[i].output)
		//~ {
			//~ PRINT_DEBUG("test|%d] failed", i);
			//~ exit(-1);
		//~ }
	//~ }
	int test1[] = {0,0,1};
	int test2[] = {0,1,1};

	struct Results result = GenomicRangeQuery("TC", test1, test2, 3);
	printArray(result.A, result.M);

	return 0;
}
