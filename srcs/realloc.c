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

/*
** Part 1 stub. Returning NULL without touching ptr matches the libc contract
** for a realloc that cannot satisfy the request: the original allocation is
** left untouched and the caller keeps ownership of it.
*/

void	*realloc(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	return (NULL);
}
