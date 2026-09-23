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
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define ALIGNMENT 16

#define TINY_MAX  128
#define SMALL_MAX 1024

#define HISTORY   64
#define DUMP_MAX  32

#define OP_MALLOC  0
#define OP_FREE    1
#define OP_REALLOC 2
#define OP_CALLOC  3

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

typedef struct s_event
{
	void    *ptr;
	size_t  size;
	int     op;
} t_event;

//jnde is here, this is the only global variable used, it is to save the allocated
//memory depending on its kind, so if I want to allocate another time, I'll check if
//there is already a zone of that kind with free space, and if not, I'll create one.
//the debug flags and the history live in the same struct so it stays ONE global.
typedef struct s_arena
{
	t_zone   *zones[3];
	t_event  history[HISTORY];
	size_t   events;
	int      scribble;
	int      trace;
	int      ready;
} t_arena;

extern t_arena         g_arena;

//jnde is here, the second global, only for the thread-safety bonus
extern pthread_mutex_t g_lock;

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

void init_debug(void);
void scribble(void *ptr, size_t size, unsigned char byte);
void record(int op, void *ptr, size_t size);

void *malloc_impl(size_t size);
void free_impl(void *ptr);
void *realloc_impl(void *ptr, size_t size);
void show_alloc_mem_impl(void);

void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);
void show_alloc_mem_ex(void);

#endif
