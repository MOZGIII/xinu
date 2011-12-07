/* x_format.c - x_format */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <name.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  x_format  --  (command format) format a file system
 *-----------------------------------------------------------------------
 */
COMMAND	x_format(nargs, args)
int	nargs;
char	*args[];
{
	int	device;
	char	*dptr;
	char	*idnump;

    if (nargs != 2) {
        xinu_fprintf(STDERR, "Usage: format idnum\n");
		return(SYSERR);
	}
    dptr   = "ds0:";
	idnump = args[1];
    if ( nargs == 2 ) {
        idnump = args[1];
	}
	if ( (device=nammapd(dptr)) == SYSERR) {
		xinu_fprintf(STDERR,"format: could not format %s\n",args[1]);
		return(SYSERR);
	}
	if ( format(device,idnump) == SYSERR ) {
		xinu_fprintf(STDERR, "Cannot format %s\n",args[1]);
		return(SYSERR);
	}
	return(OK);
}
