/* ************************************************/
/*                                                */
/*                                                */
/*   ft_malloc.h                                  */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#ifndef FT_MALLOC_H
#define FT_MALLOC_H

#include <stddef.h>

/*
** The four public symbols of the library.
** The first three must match the libc prototypes byte for byte, because the
** dynamic loader resolves them by name only: it does not check types. A
** mismatch here would not fail to build, it would corrupt every program that
** loads us.
*/

void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);

#endif
