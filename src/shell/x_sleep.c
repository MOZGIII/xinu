/* x_sleep.c - x_sleep */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  x_sleep  --  (command sleep) delay for a given number of seconds
 *------------------------------------------------------------------------
 */
COMMAND	x_sleep(nargs, args)
int	nargs;
char	*args[];
{
	if (nargs != 2) {
		xinu_fprintf(STDERR, "Usage: sleep delay\n");
		return(SYSERR);
	}
	return( sleep(atoi(args[1])) );
}
