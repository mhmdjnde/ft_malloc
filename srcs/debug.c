/* ************************************************/
/*                                                */
/*                                                */
/*   debug.c                                      */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

void	init_debug(void)
{
	g_arena.ready = 1;
	if (getenv("MALLOC_SCRIBBLE") != NULL)
		g_arena.scribble = 1;
	if (getenv("MALLOC_TRACE") != NULL)
		g_arena.trace = 1;
}

void	scribble(void *ptr, size_t size, unsigned char byte)
{
	unsigned char	*bytes;
	size_t			i;

	if (g_arena.scribble == 0 || ptr == NULL)
		return ;
	bytes = ptr;
	i = 0;
	while (i < size)
	{
		bytes[i] = byte;
		i++;
	}
}

void	record(int op, void *ptr, size_t size)
{
	char	*names[4];

	names[0] = "malloc ";
	names[1] = "free   ";
	names[2] = "realloc";
	names[3] = "calloc ";
	g_arena.history[g_arena.events % HISTORY].op = op;
	g_arena.history[g_arena.events % HISTORY].ptr = ptr;
	g_arena.history[g_arena.events % HISTORY].size = size;
	g_arena.events++;
	if (g_arena.trace == 0)
		return ;
	put_str(names[op]);
	put_str(" ");
	put_nbr(size);
	put_str(" bytes at ");
	put_ptr(ptr);
	put_str("\n");
}
