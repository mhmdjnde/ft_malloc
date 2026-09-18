/* ************************************************/
/*                                                */
/*                                                */
/*   free.c                                       */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

static int	count_zones(int kind)
{
	t_zone	*zone;
	int		count;

	count = 0;
	zone = g_zones[kind];
	while (zone != NULL)
	{
		count++;
		zone = zone->next;
	}
	return (count);
}

static t_zone	*find_zone(void *ptr)
{
	t_zone	*zone;
	int		kind;

	kind = 0;
	while (kind < 3)
	{
		zone = g_zones[kind];
		while (zone != NULL)
		{
			if ((char *)ptr > (char *)zone
				&& (char *)ptr < (char *)zone + zone->size)
				return (zone);
			zone = zone->next;
		}
		kind++;
	}
	return (NULL);
}

static t_block	*find_block(t_zone *zone, void *ptr)
{
	t_block	*block;

	block = zone->blocks;
	while (block != NULL)
	{
		if ((void *)(block + 1) == ptr)
			return (block);
		block = block->next;
	}
	return (NULL);
}

static void	merge_block(t_block *block)
{
	t_block	*other;

	if (block->next != NULL && block->next->free == 1)
	{
		other = block->next;
		block->size = block->size + sizeof(t_block) + other->size;
		block->next = other->next;
		if (block->next != NULL)
			block->next->prev = block;
	}
	if (block->prev != NULL && block->prev->free == 1)
	{
		other = block->prev;
		other->size = other->size + sizeof(t_block) + block->size;
		other->next = block->next;
		if (block->next != NULL)
			block->next->prev = other;
	}
}

static void	release_zone(t_zone *zone)
{
	t_zone	*prev;
	t_zone	*cur;

	if (zone->blocks->free == 0 || zone->blocks->next != NULL)
		return ;
	if (zone->kind != 2 && count_zones(zone->kind) == 1)
		return ;
	prev = NULL;
	cur = g_zones[zone->kind];
	while (cur != NULL && cur != zone)
	{
		prev = cur;
		cur = cur->next;
	}
	if (cur == NULL)
		return ;
	if (prev == NULL)
		g_zones[zone->kind] = zone->next;
	else
		prev->next = zone->next;
	munmap(zone, zone->size);
}

void	free(void *ptr)
{
	t_zone	*zone;
	t_block	*block;

	if (ptr == NULL)
		return ;
	zone = find_zone(ptr);
	if (zone == NULL)
		return ;
	block = find_block(zone, ptr);
	if (block == NULL || block->free == 1)
		return ;
	block->free = 1;
	merge_block(block);
	release_zone(zone);
}
