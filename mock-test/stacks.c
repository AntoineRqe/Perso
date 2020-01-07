#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>


#define COUNT_ITEMS(A) (sizeof(A)/sizeof(A[0]))
#define DEBUG (1)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, fmt "\n", __VA_ARGS__);}} while(0)

int shopping(int A[], int N)
{
    int i = 0, min = 0;
    int tail = 0;
    int front = 0;

    for(i = 0; i < N; i++)
    {
        if(A[i] == 1) // Pop
        {
            if(tail >= front) min++;
            else tail++;
        }
        else if(A[i] == 0)  // Push
        {
            front++;
        }
    }

    return min;
}

int buildWalls(int H[], int N)
{
    int i = 0;
    int nb_blocks = 1;
    int stack[100000] = { 0 };
    stack[0] = H[0];
    int stack_size = 1;

    for(i = 0; i < N; i++)
    {
        if(H[i] == stack[stack_size - 1])
            continue;
        else if(H[i] > stack[stack_size - 1])
        {
            printf("Added new wall %d\n", i);
            stack[stack_size++] = H[i];
            nb_blocks++;
        }
        else
        {
            while(stack_size > 0)
            {
                stack_size--;
                if(stack_size == 0)
                {
                    printf("Added new wall %d\n", i);
                    stack[stack_size++] = H[i];
                    nb_blocks++;
                    break;
                }
                else if(H[i] > stack[stack_size - 1])
                {
                    printf("Added new wall %d\n", i);
                    stack[stack_size++] = H[i];
                    nb_blocks++;
                    break;
                }
                else if(H[i] == stack[stack_size - 1])
                {
                    break;
                }
            }

        }
    }

    printf("min walls : %d\n", nb_blocks);
    return nb_blocks;
}

// Use of stack here
int nestedString(char * S)
{
    int i = 0;
    int index;
    char stack[200000];

    if(!S)
        return 0;

    int len = strlen(S);
    if(len == 0)
        return 1;

    for(i = 0; i < len; i++)
    {
        if((S[i] == '(') || (S[i] == '{') || (S[i] == '['))
        {
            stack[index++] = S[i];
        }
        else
        {
            if(index < 1)
                return 0;

            switch(S[i])
            {
                case ')':
                    if(stack[index - 1] == '(')
                        index--;
                    else
                        return 0;
                    break;
                case '}':
                    if(stack[index - 1] == '{')
                        index--;
                    else 
                        return 0;
                    break;
                case ']':
                    if(stack[index - 1] == '[')
                        index--;
                    else 
                        return 0;
                    break;
            }
        }
    }

    if(index != 0)
        return 0;
    return 1;
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
		{.input_tab={8,8,5,7,9,8,7,4,8}, .tab_size = 9, .output = 7},


	};

	for(i = 0; i < COUNT_ITEMS(tests); i++)
	{
		if(buildWalls(tests[i].input_tab, tests[i].tab_size) != tests[i].output)
		{
			PRINT_DEBUG("test[%d] failed", i);
			exit(-1);
		}
	}

    //~ assert(nestedString("") == 1);
    //~ assert(nestedString("{}") == 1);

	return 0;
}
