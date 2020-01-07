#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))
#define DEBUG (1)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)

// O(1)
void swap(int * a, int * b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int A[], int low, int high)  
{  
    int pivot = A[high];
    int i = (low - 1);
  
    for (int j = low; j <= high - 1; j++)  
    {  
        if (A[j] < pivot)  
        {  
            i++;
            swap(&A[i], &A[j]);  
        }  
    }  
    swap(&A[i + 1], &A[high]);  
    return (i + 1);  
}  

// O(N²) or O(NlogN)
void quicksort(int A[], int low, int high)
{
    while (low < high) 
    {
        int pi = partition(A, low, high); 
        if (pi - low < high - pi) 
        { 
            quicksort(A, low, pi - 1); 
            low = pi + 1; 
        }
        else
        { 
            quicksort(A, pi + 1, high); 
            high = pi - 1; 
        }
    }
}

int solution(int A[], int N)
{
	int i = 0;

	quicksort(A, 0, (N-1));

	for(i = 0; i < N, i++)
		if(A[i] != i)
			return A[i];

	return A[--i];
}

typedef struct test_s{
	int input[10];
	int size;
	int output;
}test_t;

int main(void)
{
	unsigned int i = 0;

	test_t tests[] = 
	{
		{{1}, 1, 1},
		{{1000000000}, 1, 0},
		{{2}, 1, 0},
		{{1,1}, 2, 0},
		{{1,2,3}, 3, 1},
		{{3,2,1}, 3, 1},
		{{4,1,3,2}, 4, 1},
		{{1,4,1}, 3, 0},
		{{4,1,3}, 3, 0}
	};

	for(i = 0; i < COUNT_ITEMS(tests); i++)
	{
		if((solution(tests[i].input, tests[i].size) != tests[i].output))
		{
			PRINT_DEBUG("test|%d] failed", i);
			exit(-1);
		}
	}

	return 0;
}
