/* ************************************************/
/*                                                */
/*                                                */
/*   zone.c                                       */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

t_zone *g_zones[3] = {NULL, NULL, NULL};

static void	add_zone(int kind, t_zone *zone)
{
	t_zone	*prev;
	t_zone	*cur;

	prev = NULL;
	cur = g_zones[kind];
	while (cur != NULL && cur < zone)
	{
		prev = cur;
		cur = cur->next;
	}
	zone->next = cur;
	if (prev == NULL)
		g_zones[kind] = zone;
	else
		prev->next = zone;
}

t_zone *create_zone(int kind, size_t size)
{
	size_t total;

	if (kind == 2)
		total = large_zone_size(size);
	else
		total = zone_size(kind);

	if (total == 0)
		return NULL;

	//jndeishere, NULL so random address, total size, read and write only,
	//private and plain memory not a file, no file descriptor, no offset	
	t_zone *zone = mmap(NULL, total, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANON, -1, 0);

	if (zone == MAP_FAILED)
		return NULL;

	zone->kind = kind;
	zone->size = total;
	zone->blocks = (t_block *)(zone + 1);

	t_block *first = zone->blocks;
	first->size = total - sizeof(t_zone) - sizeof(t_block);
	first->free = 1;
	first->prev = NULL;
	first->next = NULL;

	add_zone(kind, zone);
	return zone;
}
