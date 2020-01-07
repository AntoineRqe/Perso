#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define DEBUG (1)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)
#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))

void printArray(int arr[], int size)
{
	printf("Size : %d\n", size);
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");
}

int sum(int first_item, int last_item)
{
	if(last_item == first_item)
		return first_item;

	return last_item + sum(first_item, (last_item -1));
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void quicksort(int arr[], int l, int r)
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
            swap(&arr[cnt], &arr[i]);
            cnt++;
        }
    }
    
    quicksort(arr, l, cnt-2);
    quicksort(arr, cnt, r);
}

int solution(int A[], int N) 
{
	int i = 0;
	quicksort(A, 0, (N -1));

	if(N == 0)
		return 1;
	else if(N == 1)
	{
		if(A[0] == 1)
			return 2;
		else
			return 1;
	}

	for(i = 0; i < N; i++)
	{
		if(A[i] != (i + 1))
			return (i + 1);
	}

	return (i + 1);
}



int main(void)
{
	int test[] = {2, 3, 1, 5};
	int test3[] = {2, 4, 5, 3};
	int test4[] = {1, 3, 2, 5};
	int test5[] = {};
	int test6[] = {1};
	int test2[] = {2};
	int test7[] = {2,3};
	int test8[] = {1,2};

	assert(solution(test, COUNT_ITEMS(test)) == 4);
	assert(solution(test2, COUNT_ITEMS(test2)) == 1);
	assert(solution(test3, COUNT_ITEMS(test3)) == 1);
	assert(solution(test4, COUNT_ITEMS(test4)) == 4);
	assert(solution(test5, COUNT_ITEMS(test5)) == 1);
	assert(solution(test6, COUNT_ITEMS(test6)) == 2);
	assert(solution(test7, COUNT_ITEMS(test7)) == 1);
	assert(solution(test8, COUNT_ITEMS(test8)) == 3);

	return 0;
}
