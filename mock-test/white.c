#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

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

int solution(int A[], int N)
{
	return 0;
}

typedef struct test_s{
	int input_data;
	int input_tab[10];
	int tab_size;
	int output;
}test_t;

int main(void)
{
	//~ unsigned int i = 0;

	//~ test_t tests[] = 
	//~ {
		//~ {.input_data=1, .input_tab={1}, .tab_size = 1, .output=0},

	//~ };

	//~ for(i = 0; i < COUNT_ITEMS(tests); i++)
	//~ {
		//~ if(solution(tests[i].input_data, tests[i].input_tab, tests[i].tab_size) != tests[i].output)
		//~ {
			//~ PRINT_DEBUG("test[%d] failed", i);
			//~ exit(-1);
		//~ }
	//~ }

    size_t len = 0;
    char * substring = NULL;
    char * test = "A10 B12 C4";

    do
    {
        test = extract_string(test, " ", &substring, &len);
        if(!substring)
            break;

        printf("test : [%s]\n", test);
        printf("substring[%d] : [%s]\n", len, substring);
    }while(substring);

	return 0;
}
