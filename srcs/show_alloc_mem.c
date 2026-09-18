/* ************************************************/
/*                                                */
/*                                                */
/*   show_alloc_mem.c                             */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

static size_t	print_blocks(t_zone *zone)
{
	size_t	used;
	t_block	*block;

	used = 0;
	block = zone->blocks;
	while (block != NULL)
	{
		if (block->free == 0)
		{
			put_ptr(block + 1);
			put_str(" - ");
			put_ptr((char *)(block + 1) + block->size);
			put_str(" : ");
			put_nbr(block->size);
			put_str(" bytes\n");
			used = used + block->size;
		}
		block = block->next;
	}
	return (used);
}

void	show_alloc_mem(void)
{
	char	*names[3];
	t_zone	*zone;
	size_t	total;
	int		kind;

	names[0] = "TINY";
	names[1] = "SMALL";
	names[2] = "LARGE";
	total = 0;
	kind = 0;
	while (kind < 3)
	{
		zone = g_zones[kind];
		while (zone != NULL)
		{
			put_str(names[kind]);
			put_str(" : ");
			put_ptr(zone);
			put_str("\n");
			total = total + print_blocks(zone);
			zone = zone->next;
		}
		kind++;
	}
	put_str("Total : ");
	put_nbr(total);
	put_str(" bytes\n");
}
