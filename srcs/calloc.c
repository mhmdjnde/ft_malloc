/* ************************************************/
/*                                                */
/*                                                */
/*   calloc.c                                     */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

void	*calloc(size_t count, size_t size)
{
	void	*ptr;
	char	*bytes;
	size_t	total;
	size_t	i;

	if (count != 0 && size > SIZE_MAX / count)
		return (NULL);
	total = count * size;
	ptr = malloc(total);
	if (ptr == NULL)
		return (NULL);
	bytes = ptr;
	i = 0;
	while (i < total)
	{
		bytes[i] = 0;
		i++;
	}
	record(OP_CALLOC, ptr, total);
	return (ptr);
}
