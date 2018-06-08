#include <stdio.h>
#include <string.h>

#define FILE_PATH "/tmp/hack0.txt"

int main(int c, char v[])
{
	FILE * fd;
	char * aiguille = NULL;

	if(!(fd = fopen(FILE_PATH, "r")))
		printf("imposible to open %s\n", FILE_PATH);
	else 
		fclose(fd);
	aiguille = strstr(FILE_PATH, "hack");
	if(aiguille)
		printf("found aiguille %s\n", aiguille);
	return 0;
}
