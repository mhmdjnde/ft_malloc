/* ************************************************/
/*                                                */
/*                                                */
/*   realloc.c                                    */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

void	copy_bytes(void *dst, void *src, size_t n)
{
	char	*d;
	char	*s;
	size_t	i;

	d = dst;
	s = src;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
}

size_t	room_after(t_block *block)
{
	if (block->next != NULL && block->next->free == 1)
		return (block->size + sizeof(t_block) + block->next->size);
	return (block->size);
}

void	*resize_here(void *ptr, t_block *block, size_t size)
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
	split_block(block, size);
	return (ptr);
}

void	*move_elsewhere(void *ptr, t_block *block, size_t size)
{
	void	*fresh;
	size_t	copy;

	fresh = malloc_impl(size);
	if (fresh == NULL)
		return (NULL);
	copy = block->size;
	if (size < copy)
		copy = size;
	copy_bytes(fresh, ptr, copy);
	free_impl(ptr);
	record(OP_REALLOC, fresh, size);
	return (fresh);
}

void	*resize_large(void *ptr, t_zone *zone, t_block *block, size_t size)
{
	if (size <= zone->size - sizeof(t_zone) - sizeof(t_block))
	{
		block->size = size;
		return (ptr);
	}
	return (move_elsewhere(ptr, block, size));
}

void	*realloc_impl(void *ptr, size_t size)
{
	t_zone	*zone;
	t_block	*block;

	if (ptr == NULL)
		return (malloc_impl(size));
	if (size == 0)
	{
		free_impl(ptr);
		return (NULL);
	}
	if (size > SIZE_MAX - ALIGNMENT)
		return (NULL);
	zone = find_zone(ptr);
	if (zone == NULL)
		return (NULL);
	block = find_block(zone, ptr);
	if (block == NULL || block->free == 1)
		return (NULL);
	size = align_up(size, ALIGNMENT);
	if (get_kind(size) != zone->kind)
		return (move_elsewhere(ptr, block, size));
	if (zone->kind == 2)
		return (resize_large(ptr, zone, block, size));
	if (room_after(block) >= size)
		return (resize_here(ptr, block, size));
	return (move_elsewhere(ptr, block, size));
}

void	*realloc(void *ptr, size_t size)
{
	void	*fresh;

	pthread_mutex_lock(&g_lock);
	fresh = realloc_impl(ptr, size);
	pthread_mutex_unlock(&g_lock);
	return (fresh);
}
