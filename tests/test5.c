/* error management: every one of these must be survived without a crash, and
   realloc must answer NULL when it cannot satisfy the request. If the argument
   is still printed at the end, nothing blew up.
   run: ./run.sh ./test5 Bonjour */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv)
{
	char	*addr;
	int		stack_var;

	free(NULL);
	free((void *)0x1234);
	stack_var = 7;
	free(&stack_var);

	addr = (char *)malloc(0);
	free(addr);

	addr = (char *)realloc(NULL, 16);
	if (addr == NULL)
		return (1);
	addr[0] = 'x';

	if (realloc(addr, SIZE_MAX) != NULL)
		printf("realloc(SIZE_MAX) should have returned NULL\n");
	if (addr[0] != 'x')
		printf("a failed realloc must not touch the original block\n");

	if (realloc((void *)0x1234, 64) != NULL)
		printf("realloc on a foreign pointer should have returned NULL\n");
	if (realloc(&stack_var, 64) != NULL)
		printf("realloc on a stack pointer should have returned NULL\n");

	if (realloc(addr, 0) != NULL)
		printf("realloc(ptr, 0) should have returned NULL\n");

	if (malloc(SIZE_MAX) != NULL)
		printf("malloc(SIZE_MAX) should have returned NULL\n");

	if (argc > 1)
		printf("%s\n", argv[1]);
	else
		printf("pass an argument, it should be echoed back\n");
	return (0);
}
