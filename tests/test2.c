/* same as test1 but freeing every block, so the page count should drop back
   down to roughly test0 */

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
		free(addr);
		i++;
	}
	printf("%.2f\n", (double)(clock() - start) / CLOCKS_PER_SEC);
	return (0);
}
