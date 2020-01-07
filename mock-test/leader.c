#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))
#define DEBUG (1)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)


int dominator(int A[], int N)
{
    int i = 0;
    int value;
    int size = 0;
    int count = 0;
    int candidate = -1;
    int offset = -1;

    for(i = 0; i < N; i++)
    {
        if(size == 0)
        {
            value = A[i];
            size++;
        }
        else
        {
            if(A[i] != value)
                size--;
            else
                size++;
        }
    }

    if(size > 0)
        candidate = value;

    for(i = 0; i < N; i++)
        if(candidate == A[i])
        {
            count++;
            if(index < 0)
                index = i;
        }

    if(count <= N/2)
        index = -1;

    return index;
}


static int goldenLeader(int A[], int N)
{
    int i           = 0;
    int size        = 0;
    int value       = 0;
    int leader      = -1;
    int candidate   = -1;
    int count       = 0;

    for(i = 0; i < N; i++)
    {
        if(size == 0)
        {
            value = A[i];
            size++;
        }
        else
        {
            if(A[i] != value)
                size--;
            else
                size++;
        }
    }

    if(size > 0)
        candidate = value;

    for(i = 0; i < N; i++)
    {
        if(A[i] == candidate)
            count++;
    }

    if(count > N/2)
        leader = candidate;

    return leader;
}

int EquiLeader(int A[], int N)
{
    int i, S = 0;
    int true_leader = -1;
    int * leader_count = (int*)malloc(sizeof(*leader_count) * N);
    
    int count = 0;

    true_leader = goldenLeader(A, N);

    for(i = 0; i < N; i++)
    {
        if(A[i] == true_leader)
            count++;
        leader_count[i] = count;
    }

    printArray(leader_count, N);

    count = 0;
    int left_size = 0;
    int right_size = 0;
    int left_count = 0;
    int right_count = 0;

    for(S = 0; S < N; S++)
    {
        left_count = leader_count[S];
        right_count = leader_count[(N-1)] - left_count;
        left_size = S+1;
        right_size = N - left_size;

        if((left_count > left_size/2) && (right_count > (right_size/2)))
            count++;
    }

    return count;
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
			PRINT_DEBUG("test[%d] failed", i);
			exit(-1);
		}
	}
}
