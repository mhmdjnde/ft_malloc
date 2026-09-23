/* show_alloc_mem: one allocation in each class so the display has a TINY, a
   SMALL and a LARGE section
   build: gcc -o test6 test6.c -L. -lft_malloc
   run:   ./run.sh ./test6 */

#include <stdlib.h>
#include <string.h>

void	show_alloc_mem(void);

int main(void)
{
	char	*tiny;
	char	*small;
	char	*large;

	tiny = (char *)malloc(42);
	small = (char *)malloc(512);
	large = (char *)malloc(1024 * 1024);
	if (tiny == NULL || small == NULL || large == NULL)
		return (1);
	memset(tiny, 'T', 42);
	memset(small, 'S', 512);
	memset(large, 'L', 1024 * 1024);
	show_alloc_mem();
	free(tiny);
	free(small);
	free(large);
	return (0);
}
