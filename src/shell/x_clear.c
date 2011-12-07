/* x_clear.c - x_clear */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <name.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  x_clear  --  (command clear) clear the named window (STDOUT default)
 *------------------------------------------------------------------------
 */
COMMAND	x_clear(nargs, args)
int	nargs;
char	*args[];
{	
	int	device;
	int	dtyp;
	struct	devsw	*devptr;
	struct	tty	*ttyptr;

	if (nargs > 2) {
		xinu_fprintf(STDERR,"Usage: clear [window]\n");
		return(SYSERR);
	}
	if (nargs == 2) {
		device = nammapd(args[1]);
		if (device == SYSERR) {
			xinu_fprintf(STDERR,"clear: unknown device %s\n",args[1]);
			return(SYSERR);
		}
	} else
		device = getpdev(xinu_getpid(), STDOUT);
	if (!isatty(device)) {
		xinu_fprintf(STDERR,"clear: %s is not a window\n",args[1]);
		return(SYSERR);
	}
	control(device,TCCLEAR);
	return(OK);
}
