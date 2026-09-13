/* ************************************************/
/*                                                */
/*                                                */
/*   malloc.c                                     */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

t_block *find_free_block(int kind, size_t size)
{
	t_zone *zone = g_zones[kind];

	while (zone)
	{
		t_block *block = zone->blocks;

		while (block)
		{
			if (block->free && block->size >= size)
				return block;
			block = block->next;
		}
		zone = zone->next;
	}
	return NULL;
}

// cuts the tail of a block loose as a new free block. only worth doing if the tail
// can hold its own header plus a payload someone could actually use, otherwise we
// leave the block whole and the caller gets a few bytes more than asked for.

void split_block(t_block *block, size_t size)
{
	if (block->size < size + sizeof(t_block) + ALIGNMENT)
		return;

	t_block *tail = (t_block *)((char *)(block + 1) + size);

	tail->size = block->size - size - sizeof(t_block);
	tail->free = 1;
	tail->prev = block;
	tail->next = block->next;
	if (tail->next)
		tail->next->prev = tail;

	block->size = size;
	block->next = tail;
}

void *malloc(size_t size)
{
	if (size == 0)
		size = 1;

	if (size > SIZE_MAX - ALIGNMENT)
		return NULL;

	size = align_up(size, ALIGNMENT);

	int kind = get_kind(size);
	if (kind == 2)
		return NULL;

	t_block *block = find_free_block(kind, size);
	if (block == NULL)
	{
		if (create_zone(kind, size) == NULL)
			return NULL;
		block = find_free_block(kind, size);
		if (block == NULL)
			return NULL;
	}

	split_block(block, size);
	block->free = 0;

	return block + 1;
}
