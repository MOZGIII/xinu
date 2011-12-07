/* blkcopy.c - blkcopy */

#include <kernel.h>

/*------------------------------------------------------------------------
 *  blkcopy  -  copy a block of memory form one location to another
 *------------------------------------------------------------------------
 */
blkcopy(to, from, nbytes)
register char	*to;
register char	*from;
register int	nbytes;
{
	if (nbytes > 0) {
		while (--nbytes >= 0)
			*to++ = *from++;
		return(OK);
	}
	return(SYSERR);
}
