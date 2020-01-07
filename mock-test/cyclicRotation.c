#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))
#define DEBUG (0)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)

struct Results {
  int * A;
  int 	N; // Length of the array
};

struct Results solution(int A[], int N, int K) {
	int i = 0;
    struct Results result;

	int * tmp_A = (int *)malloc(sizeof(*A) * N);

	for(i = 0; i < N; i++)
	{
		int rotate_index = (K + i) % N;
		tmp_A[rotate_index] = A[i];
	}

	result.A = tmp_A;
	result.N = N;

    return result;
}

int main(void)
{
	int test[] = {1};
	int test1[] = {1,1};
	int test2[] = {1,1,1};
	int test3[] = {1,2,3};
	struct Results result;

	// test
	result = solution(test, COUNT_ITEMS(test), 0);
	if(memcmp(result.A, test, COUNT_ITEMS(test)))
		assert(0);

	result = solution(test, COUNT_ITEMS(test), 10);
	if(memcmp(result.A, test, COUNT_ITEMS(test)))
		assert(0);

	// test1
	result = solution(test1, COUNT_ITEMS(test1), 0);
	if(memcmp(result.A, test1, COUNT_ITEMS(test1)))
		assert(0);

	result = solution(test1, COUNT_ITEMS(test1), 0);
	if(memcmp(result.A, test1, COUNT_ITEMS(test1)))
		assert(0);

	// test2
	result = solution(test2, COUNT_ITEMS(test2), 0);
	if(memcmp(result.A, test2, COUNT_ITEMS(test2)))
		assert(0);

	result = solution(test2, COUNT_ITEMS(test2), 0);
	if(memcmp(result.A, test2, COUNT_ITEMS(test2)))
		assert(0);

	//test3
	result = solution(test3, COUNT_ITEMS(test3), 0);
	if(memcmp(result.A, test3, COUNT_ITEMS(test3)))
		assert(0);

	int result_test3_K1[] = {3,1,2};
	int result_test3_K2[] = {2,3,1};
	int result_test3_K3[] = {1,2,3};

	result = solution(test3, COUNT_ITEMS(test3), 1);
	if(memcmp(result.A, result_test3_K1, COUNT_ITEMS(test3)))
		assert(0);

	result = solution(test3, COUNT_ITEMS(test3), 2);
	if(memcmp(result.A, result_test3_K2, COUNT_ITEMS(test3)))
		assert(0);

	result = solution(test3, COUNT_ITEMS(test3), 3);
	if(memcmp(result.A, result_test3_K3, COUNT_ITEMS(test3)))
		assert(0);

	result = solution(test3, COUNT_ITEMS(test3), 5);
	if(memcmp(result.A, result_test3_K2, COUNT_ITEMS(test3)))
		assert(0);

	result = solution(test3, COUNT_ITEMS(test3), 6);
	if(memcmp(result.A, result_test3_K3, COUNT_ITEMS(test3)))
		assert(0);

	//test 4
	int test4[] = {1, 2, 3, 4, 5, 6, 7};
	int result_test4_K1[] = {7, 1, 2, 3, 4, 5, 6};
	int result_test4_K2[] = {6, 7, 1, 2, 3, 4, 5};

	result = solution(test4, COUNT_ITEMS(test4), 1);
	if(memcmp(result.A, result_test4_K1, COUNT_ITEMS(test4)))
		assert(0);

	result = solution(test4, COUNT_ITEMS(test4), 2);
	if(memcmp(result.A, result_test4_K2, COUNT_ITEMS(test4)))
		assert(0);

	return 0;
}
