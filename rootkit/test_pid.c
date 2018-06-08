#include <sys/types.h>
#include <unistd.h>
#include <stdio.h> 

int main(int c, char v[])
{
	pid_t cur = getpid();
	pid_t par = getppid();

	printf("cur[%d] par[%d]\n", cur, par);
	return 0;
}
