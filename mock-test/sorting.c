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

struct disc
{
    int center;
    int x_min;
    int x_max;
};

int solution(int A[], int N)
{
    long int nb_intersection = 0;
    int i = 0, j = 0;

    struct disc * discs = (struct disc *)malloc(N * sizeof(*discs));

    for(i = 0; i < N; i++)
    {
        discs[i].center = i;
        discs[i].x_min = i - A[i];
        discs[i].x_max = i + A[i];
    }

    for(i = 0; i < N; i++)
    {
        for(j = (i + 1); j < N; j++)
        {
            if(discs[j].x_min <= discs[i].x_max)
                nb_intersection++;
        }

        if(nb_intersection > 10000000)
            return -1;
    }

	return nb_intersection;
}


typedef struct test_s{
	int input_data;
	int input_tab[10];
	int tab_size;
	int output;
}test_t;

int main(void)
{
	unsigned int i = 0;

	test_t tests[] = 
	{
		{.input_data=1, .input_tab={1}, .tab_size = 1, .output=0},

	};

	for(i = 0; i < COUNT_ITEMS(tests); i++)
	{
		if(solution(tests[i].input_data, tests[i].input_tab, tests[i].tab_size) != tests[i].output)
		{
			PRINT_DEBUG("test|%d] failed", i);
			exit(-1);
		}
	}

	return 0;
}
