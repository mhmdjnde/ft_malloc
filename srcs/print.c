/* ************************************************/
/*                                                */
/*                                                */
/*   print.c                                      */
/*                                                */
/*   By: JndeIsBack                               */
/*                                                */
/*   Created: 2026/09/10 00:00:00 by JndeIsBack   */
/*   Updated: 2029/11/99 23:33:56 by JndeIsBack   */
/*                                                */
/* ************************************************/

#include "ft_malloc.h"

int	digcount(size_t n, size_t base)
{
	int	count;

	count = 0;
	while (n != 0)
	{
		count++;
		n = n / base;
	}
	return (count);
}

void	put_str(char *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	write(1, s, len);
}

void	put_nbr(size_t n)
{
	char	buf[20];
	int		len;
	int		i;

	if (n == 0)
	{
		write(1, "0", 1);
		return ;
	}
	len = digcount(n, 10);
	i = len - 1;
	while (n != 0)
	{
		buf[i] = n % 10 + '0';
		n = n / 10;
		i--;
	}
	write(1, buf, len);
}

void	put_ptr(void *ptr)
{
	char	*hex;
	char	buf[16];
	size_t	n;
	int		len;
	int		i;

	hex = "0123456789ABCDEF";
	n = (size_t)ptr;
	write(1, "0x", 2);
	if (n == 0)
	{
		write(1, "0", 1);
		return ;
	}
	len = digcount(n, 16);
	i = len - 1;
	while (n != 0)
	{
		buf[i] = hex[n % 16];
		n = n / 16;
		i--;
	}
	write(1, buf, len);
}
