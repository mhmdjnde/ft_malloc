/* realloc: grow one buffer by appending every argument to it
   run: ./run.sh ./test3 Bonjour Bonjour   ->  BonjourBonjour */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char	*addr;
	size_t	len;
	size_t	n;
	int		i;

	addr = NULL;
	len = 0;
	i = 1;
	while (i < argc)
	{
		n = strlen(argv[i]);
		addr = (char *)realloc(addr, len + n + 1);
		if (addr == NULL)
			return (1);
		memcpy(addr + len, argv[i], n);
		len = len + n;
		addr[len] = '\0';
		i++;
	}
	if (addr != NULL)
		printf("%s\n", addr);
	free(addr);
	return (0);
}
