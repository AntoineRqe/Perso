#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))

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
void quicksort(int arr[], int low, int high)
{
    while (low < high) 
    {
        int pi = partition(arr, low, high); 
        if (pi - low < high - pi) 
        { 
            quicksort(arr, low, pi - 1); 
            low = pi + 1; 
        }
        else
        { 
            quicksort(arr, pi + 1, high); 
            high = pi - 1; 
        }
    }
}

int solution(int A[], int N) {
	int i = 0;
	quicksort(A, 0, N-1);

	while(i < N)
	{
		if(A[i] != A[i+1])
			return A[i];
		i += 2;
	}
	return A[N -1];
}

int main(void)
{
	int test0[] = {1,1,2,2,3};
	int test1[] = {1,1,2,3,3};
	int test2[] = {1,1,3,2,3};
	int test3[] = {1,2,3,2,3};
	int test4[] = {1,1,1,1,3};
	int test5[] = {3,1,1,1,1};

	assert(solution(test0, COUNT_ITEMS(test0)) == 3);
	assert(solution(test1, COUNT_ITEMS(test1)) == 2);
	assert(solution(test2, COUNT_ITEMS(test2)) == 2);
	assert(solution(test3, COUNT_ITEMS(test3)) == 1);
	assert(solution(test4, COUNT_ITEMS(test4)) == 3);
	assert(solution(test5, COUNT_ITEMS(test5)) == 3);
}
