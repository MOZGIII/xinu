/* x_close.c - x_close */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <ctype.h>

/*------------------------------------------------------------------------
 *  x_close  -  (command close) close a device given its id
 *------------------------------------------------------------------------
 */
COMMAND	x_close(nargs, args)
int	nargs;
char	*args[];
{
	int	dev;

	if (nargs != 2) {
		xinu_fprintf(STDERR, "Usage: close device-number\n");
		return(SYSERR);
	}
	if (!isdigit(*args[1]) || (dev=atoi(args[1])) == 0 || isbaddev(dev)) {
		xinu_fprintf(STDERR, "close: bad device number %s\n", args[1]);
		return(SYSERR);
	}
	return(xinu_close(dev));
}
