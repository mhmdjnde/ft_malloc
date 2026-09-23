/* ************************************************/
/*                                                */
/*                                                */
/*   show_alloc_mem_ex.c                          */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

static void	dump_hex(unsigned char *bytes, size_t n)
{
	char	*hex;
	char	out[3];
	size_t	i;

	hex = "0123456789ABCDEF";
	i = 0;
	while (i < n)
	{
		out[0] = hex[bytes[i] / 16];
		out[1] = hex[bytes[i] % 16];
		out[2] = ' ';
		write(1, out, 3);
		i++;
	}
	while (i < DUMP_MAX)
	{
		write(1, "   ", 3);
		i++;
	}
	write(1, "|", 1);
	i = 0;
	while (i < n)
	{
		if (bytes[i] >= 32 && bytes[i] < 127)
			write(1, bytes + i, 1);
		else
			write(1, ".", 1);
		i++;
	}
	put_str("|\n");
}

static void	dump_block(t_block *block)
{
	size_t	n;

	put_str("    ");
	put_ptr(block + 1);
	put_str(" - ");
	put_ptr((char *)(block + 1) + block->size);
	put_str(" : ");
	put_nbr(block->size);
	if (block->free == 1)
	{
		put_str(" bytes free\n");
		return ;
	}
	put_str(" bytes in use\n");
	n = block->size;
	if (n > DUMP_MAX)
		n = DUMP_MAX;
	put_str("      ");
	dump_hex((unsigned char *)(block + 1), n);
}

static void	dump_zone(char *name, t_zone *zone)
{
	t_block	*block;
	size_t	used;
	size_t	holes;

	used = 0;
	holes = 0;
	block = zone->blocks;
	while (block != NULL)
	{
		if (block->free == 1)
			holes++;
		else
			used = used + block->size;
		block = block->next;
	}
	put_str(name);
	put_str(" : ");
	put_ptr(zone);
	put_str("  mapped ");
	put_nbr(zone->size);
	put_str(", in use ");
	put_nbr(used);
	put_str(", free blocks ");
	put_nbr(holes);
	put_str("\n");
	block = zone->blocks;
	while (block != NULL)
	{
		dump_block(block);
		block = block->next;
	}
}

static void	dump_history(void)
{
	char	*names[4];
	size_t	shown;
	size_t	first;
	size_t	i;

	names[0] = "malloc ";
	names[1] = "free   ";
	names[2] = "realloc";
	names[3] = "calloc ";
	shown = g_arena.events;
	if (shown > HISTORY)
		shown = HISTORY;
	first = g_arena.events - shown;
	put_str("\nhistory, last ");
	put_nbr(shown);
	put_str(" of ");
	put_nbr(g_arena.events);
	put_str(" operations\n");
	i = 0;
	while (i < shown)
	{
		put_str("    ");
		put_str(names[g_arena.history[(first + i) % HISTORY].op]);
		put_str(" ");
		put_nbr(g_arena.history[(first + i) % HISTORY].size);
		put_str(" bytes at ");
		put_ptr(g_arena.history[(first + i) % HISTORY].ptr);
		put_str("\n");
		i++;
	}
}

void	show_alloc_mem_ex(void)
{
	char	*names[3];
	t_zone	*zone;
	int		kind;

	pthread_mutex_lock(&g_lock);
	names[0] = "TINY";
	names[1] = "SMALL";
	names[2] = "LARGE";
	kind = 0;
	while (kind < 3)
	{
		zone = g_arena.zones[kind];
		while (zone != NULL)
		{
			dump_zone(names[kind], zone);
			zone = zone->next;
		}
		kind++;
	}
	dump_history();
	pthread_mutex_unlock(&g_lock);
}
