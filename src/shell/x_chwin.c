/* x_chwin.c - x_chwin */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <name.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  x_chwin  --  (command chwin) change the current window of the cursor
 *------------------------------------------------------------------------
 */
BUILTIN x_chwin(nargs, args)
int	nargs;
char	*args[];
{
	int	device;
	int	dtyp;
	char	*newnam;
	struct	tty  *ttyp;
	
	if ( nargs != 2 ) {
		xinu_fprintf(STDERR,"Usage: chwin window\n");
		return(SYSERR);
	}
	if ((device=nammapd(args[1])) == SYSERR) {
		xinu_fprintf(STDERR,"chwin: unknown device %s\n",args[1]);
		return(SYSERR);
	}
	if (!isatty(device)) {
		xinu_fprintf(STDERR,"chwin: %s is not a window\n", args[1]);
		return(SYSERR);
	}
	winofcur = devtab[device].dvminor;
	send(tty[winofcur].oprocnum,TMSGOK);
	return(OK);
}
