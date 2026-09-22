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
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

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

//jnde is here, this is the only global variable used, it is to save the allocated
//memory depending on its kind, so if I want to allocate another time, I'll check if
//there is already a zone of that kind with free space, and if not, I'll create one.
extern t_zone *g_zones[3];

size_t align_up(size_t size, size_t step);
int    get_kind(size_t size);
int    zone_size(int kind);
size_t large_zone_size(size_t size);

void put_str(char *s);
void put_nbr(size_t n);
void put_ptr(void *ptr);

t_zone  *create_zone(int kind, size_t size);
t_zone  *find_zone(void *ptr);
t_block *find_block(t_zone *zone, void *ptr);
void    split_block(t_block *block, size_t size);

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);

#endif
