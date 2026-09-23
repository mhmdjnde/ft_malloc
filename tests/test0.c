/* baseline: no malloc at all, so we have a number to compare against */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void)
{
	clock_t start;

	start = clock();
	printf("%.2f\n", (double)(clock() - start) / CLOCKS_PER_SEC);
	return (0);
}
