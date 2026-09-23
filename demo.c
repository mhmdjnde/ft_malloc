/* ************************************************/
/*                                                */
/*                                                */
/*   demo.c                                       */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include <stdlib.h>

void	show_alloc_mem(void);

int	main(void)
{
	char	*p1;
	char	*p2;
	char	*big;

	p1 = malloc(256);
	p2 = malloc(512);
	big = malloc(5000);

	p1 = realloc(p1, 256);
	p2 = realloc(p2, 1024);
	p1 = realloc(p1, 1024);

	free(p1);
	free(p2);
	free(big);

	show_alloc_mem();
	return (0);
}
