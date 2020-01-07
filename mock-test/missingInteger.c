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


void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void quicksort(int arr[], int l, int r, int *is_single)
{
    if (l >= r)
    {
        return;
    }
    
    int pivot = arr[r];

    int cnt = l;

    for (int i = l; i <= r; i++)
    {
        if (arr[i] <= pivot)
        {
			if(arr[i] == pivot)
				if(is_single)
					*is_single = 0;

            swap(&arr[cnt], &arr[i]);
            cnt++;
        }
    }
    
    quicksort(arr, l, cnt-2, is_single);
    quicksort(arr, cnt, r, is_single);
}

int solution(int A[], int N)
{
	int i = 0;
	int index = 1;

	quicksort(A, 0, N-1, NULL);

	while(A[i] <= 0)
		i++;

	if(i == N)
		return 1;
	else if(A[i] > 1)
		return 1;
	else
	{
		int j;
		
		for(j = i; j < N; j++)
		{
			if(A[j] == index)
				index++;
			else if((A[j] - index) > 2)
				return index;
		}
	}

	return index;
}

typedef struct test_s{
	int input_data;
	int input_tab[10];
	int tab_size;
	int output;
}test_t;

int main(void)
{
	int i = 0;

	test_t tests[] = 
	{
		{.input_data=1, .input_tab={-1000000,2,1000000}, .tab_size = 3, .output=1},
		{.input_data=1, .input_tab={-1000000,1,1000000}, .tab_size = 3, .output=2},
		{.input_data=1, .input_tab={4,2,3}, .tab_size = 3, .output=1},
		{.input_data=1, .input_tab={1,2,3}, .tab_size = 3, .output=4},
		{.input_data=1, .input_tab={-1,-3}, .tab_size = 2, .output=1}

	};

	for(i = 0; i < COUNT_ITEMS(tests); i++)
	{
		if(solution(tests[i].input_tab, tests[i].tab_size) != tests[i].output)
		{
			PRINT_DEBUG("test|%d] failed", i);
			exit(-1);
		}
	}

	return 0;
}
