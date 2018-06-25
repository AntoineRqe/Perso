#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int c, char* v[])
{
#define MAX_LINE	128
	char s[] = "hello, this is, it, Antoine, Bye";
	char *p = s;
	char * tok = NULL;

	while(p)
	{
		tok = strsep(&p, ",");
		printf("Extract %s[%zu]\n", tok, strlen(tok));
	}
	return 0;
}
