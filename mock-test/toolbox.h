#include <stdio.h>
#include <stdlib.h>

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

// O(N)
static int * PrefixCount(int A[], int N)
{
	int 	i = 0;
	int *	P = (int*)malloc(sizeof(int) * N);

	for(i = 0; i < N; i++)
	{
		if(i == 0)
			P[i] = A[i];
		else
			P[i] = A[i] + P[(i-1)];
	}

	return P;
}

// O(1)
static int ArithmeticSuite(int N)
{
	return (N * (N+1) / 2);
}

// O(N)
void SetAllArray(int A[], int N, int val)
{
	int i = 0;

	for(i = 0; i < N; i++)
		A[i] = val;
}

void ReverseArray(int A[], int N)
{
	int i = 0;

	for(i = 0; i < (N/2); i++)
		swap(A[i], A[(N-1-i)]);
}

// O(N)
void printArray(int A[], int N)
{
	int i =0;

    for(i = 0; i < N; i++)
        printf("%d ", A[i]);
    printf("\n");
}

static char * extract_string(char * string, char * separator, char ** substring, size_t * len)
{
    *substring = NULL;
    *len = 0;

    if(!string) return NULL;
    else if(string[0] == '\0') return NULL;

    char * needle = strstr(string, separator);

    if(!needle)
    {
        *substring = string;
        *len = strlen(string);
        return NULL;
    }

    size_t tmp_len = needle - string;

    char * tmp_substring = (char*)malloc(sizeof(char) * (tmp_len + 1));
    strncpy(tmp_substring, string, tmp_len);
    tmp_substring[tmp_len] = '\0';

    *substring = tmp_substring;
    *len = tmp_len;

    return (++needle);
}
