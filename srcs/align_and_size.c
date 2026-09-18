/* ************************************************/
/*                                                */
/*                                                */
/*   align_and_size.c                             */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

size_t align_up(size_t size, size_t step)
{
	return (size + step - 1) / step * step;
}

int get_kind(size_t size)
{
	if (size <= TINY_MAX)
		return 0;
	if (size <= SMALL_MAX)
		return 1;
	return 2;
}

int zone_size(int kind)
{
	int max_alloc = TINY_MAX;

	if (kind == 1)
		max_alloc = SMALL_MAX;

	size_t needed = sizeof(t_zone) + 100 * (max_alloc + sizeof(t_block));

	return (int)align_up(needed, getpagesize());
}

size_t large_zone_size(size_t size)
{
	size_t page = getpagesize();
	size_t overhead = sizeof(t_zone) + sizeof(t_block);

	if (size > SIZE_MAX - ALIGNMENT)
		return 0;

	size_t payload = align_up(size, ALIGNMENT);

	if (payload > SIZE_MAX - overhead - page)
		return 0;

	return align_up(payload + overhead, page);
}
