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
	t_zone *zone = g_arena.zones[kind];

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

t_block *alloc_large(size_t size)
{
	t_zone *zone = create_zone(2, size);

	if (zone == NULL)
		return NULL;

	zone->blocks->size = size;

	return zone->blocks;
}

t_block *alloc_pooled(int kind, size_t size)
{
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

	return block;
}

void *malloc_impl(size_t size)
{
	if (g_arena.ready == 0)
		init_debug();

	if (size == 0)
		size = 1;

	if (size > SIZE_MAX - ALIGNMENT)
		return NULL;

	//jndeishere, this call here because malloc always
	//return a size on a 16 byte boundary
	size = align_up(size, ALIGNMENT);

	int kind = get_kind(size);
	t_block *block;

	if (kind == 2)
		block = alloc_large(size);
	else
		block = alloc_pooled(kind, size);

	if (block == NULL)
		return NULL;

	block->free = 0;
	scribble(block + 1, block->size, 0xAA);
	record(OP_MALLOC, block + 1, block->size);

	return block + 1;
}

void	*malloc(size_t size)
{
	void	*ptr;

	pthread_mutex_lock(&g_lock);
	ptr = malloc_impl(size);
	pthread_mutex_unlock(&g_lock);
	return (ptr);
}
