/* test3 with a twist: the buffer is grown one byte at a time across all three
   size classes, so realloc has to move it between zones and keep the contents
   run: ./run.sh ./test4 Bonjour Bonjour */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char	*addr;
	size_t	len;
	size_t	i;

	addr = (char *)malloc(1);
	if (addr == NULL)
		return (1);
	addr[0] = '\0';
	len = 0;
	while (len < 5000)
	{
		addr = (char *)realloc(addr, len + 2);
		if (addr == NULL)
			return (1);
		addr[len] = 'a' + (char)(len % 26);
		len++;
		addr[len] = '\0';
	}
	i = 0;
	while (i < len)
	{
		if (addr[i] != 'a' + (char)(i % 26))
		{
			printf("corrupted at %zu\n", i);
			return (1);
		}
		i++;
	}
	printf("%zu bytes grown one at a time, all intact\n", len);
	if (argc > 1)
		printf("%s\n", argv[1]);
	free(addr);
	return (0);
}
