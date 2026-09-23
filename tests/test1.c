/* 1024 allocations of 1024 bytes = 1 Mb, writing into each one so the MMU
   really maps the page */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void)
{
	clock_t	start;
	char	*addr;
	int		i;

	start = clock();
	i = 0;
	while (i < 1024)
	{
		addr = (char *)malloc(1024);
		if (addr == NULL)
			return (1);
		addr[0] = 42;
		i++;
	}
	printf("%.2f\n", (double)(clock() - start) / CLOCKS_PER_SEC);
	return (0);
}
