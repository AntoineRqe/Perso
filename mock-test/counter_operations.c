#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

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

struct Results {
  int * C;
  int L; // Length of the array
};

struct Results solution(int N, int A[], int M) {
	int i = 0, j = 0;
	int max = 0;
    struct Results result;

	int counter[100] = { 0 };

	for(i = 0; i < M; i++)
	{
		if(A[i] <= N)
		{
			int index = (A[i] - 1);
			counter[index] += 1;
			max = (counter[index] > max) ? counter[index] : max;
		}
		else if(A[i] == (N +1))
		{
			for(j = 0; j < N; j++)
				counter[j] = max;
		}
		else
		{
			continue;
		}
	}

    result.C = counter;
    result.L = N;

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
	#define MAX (100)

	int i = 0;
	int test[MAX];
	srand(time(NULL));   // Initialization, should only be called once.

	for(i = 0; i < MAX; i++)
		test[i] = (rand() % (MAX -1)) + 1;

	solution(MAX, test, COUNT_ITEMS(test));
	//~ int i = 0;

	//~ test_t tests[] = 
	//~ {
		//~ {.input_data=5, .input_tab={3,4,4,6,1,4,4}, .tab_size = 7, .output=0},:q!
	//~ };

	//~ for(i = 0; i < COUNT_ITEMS(tests); i++)
	//~ {
		//~ if(solution(tests[i].input_data, tests[i].input_tab, tests[i].size) != tests[i].output)
		//~ {
			//~ PRINT_DEBUG("test|%d] failed", i);
			//~ exit(-1);
		//~ }
	//~ }

	return 0;
}
