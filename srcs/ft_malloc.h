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
#include <unistd.h>

#define ALIGNMENT 16

#define TINY_MAX  128
#define SMALL_MAX 1024

typedef struct s_block
{
	size_t          size;
	int             free;
	struct s_block  *prev;
	struct s_block  *next;
} t_block;

typedef struct s_zone
{
	int             kind;
	size_t          size;
	t_block         *blocks;
	struct s_zone   *next;
} t_zone;

size_t align_up(size_t size, size_t step);
int    get_kind(size_t size);
int    zone_size(int kind);
size_t large_zone_size(size_t size);

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);

#endif
