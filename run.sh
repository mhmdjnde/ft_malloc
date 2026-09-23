#!/bin/sh
export LD_LIBRARY_PATH=.
export LD_PRELOAD=libft_malloc.so
$@

#cc -o demo demo.c -L. -lft_malloc
