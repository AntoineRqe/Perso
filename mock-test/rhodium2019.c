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

typedef struct road_s
{
    int village_a;
    int village_a;
} road;

typedef struct village_s
{
    int index;
    int * roads;
    int roads_count;
} village;

static village * parse_road_map(int T[], int N)
{
    int i = 0;
    village * tmp_village = (village *)malloc(sizeof(*tmp_village) * N);

    for(i = 0; i < N; i++)
    {
        road tmp_road = {.village_a = i; .village_b = T[i]}
    }
}

int solution(int T[], int N) {
    int i = 0, j = 0;

    for(i = 0; i < N; i++)
    {
        for(j = i; j < N; j++)
        {
            
        }
    }
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
