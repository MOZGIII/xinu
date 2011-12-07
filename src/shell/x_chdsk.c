/* x_chdsk.c - x_chdsk */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <name.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  x_chdsk  -  (command chdsk) change disk drives
 *------------------------------------------------------------------------
 */
COMMAND	x_chdsk(nargs, args)
int	nargs;
char	*args[];
{	
	int	device;
	char	*drvp;
	
	if (nargs > 2) {
		xinu_fprintf(STDERR, "usage: chdsk [drive]\n");
		return(SYSERR);
	}
    drvp = "ds0";           /* default drive */
 	if (nargs == 2)
		drvp = args[1];
	if ((device=nammapd(drvp)) == SYSERR) {
		xinu_fprintf(STDERR,"chdsk: unknown device %s\n",drvp);
		return(SYSERR);
	}
    if (chdsk(device) == SYSERR) {
		xinu_fprintf(STDERR, "chdsk: cannot change disk in %s\n",drvp);
		return(SYSERR);
	}
	return(OK);
}
