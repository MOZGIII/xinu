/* x_redraw.c - x_redraw */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <name.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  x_redraw  --  (command redraw) redraw the named window (STDOUT default)
 *------------------------------------------------------------------------
 */
BUILTIN x_redraw(nargs, args)
int	nargs;
char	*args[];
{
	int	device;
	char	*wnamp;
	struct	tty  *ttyp;
	CURSOR	box[2];
	int	drawborder();
	
	if ( nargs>2 ) {
		xinu_fprintf(STDERR,"Usage: redraw [window]\n");
		return(SYSERR);
	}
	if (nargs==2) {
		device = nammapd(args[1]);
		if (device == SYSERR) {
			xinu_fprintf(STDERR,"redraw: unknown device %s\n",args[1]);
			return(SYSERR);
		}
		wnamp = args[1];
	} else {
		wnamp = NULL;
		device = getpdev(xinu_getpid(), STDOUT);
	}
	if (!isatty(device)) {
		xinu_fprintf(STDERR,"redraw: %s is not a window\n",args[1]);
		return(SYSERR);
	}
	/* poke around directly into the tty device structure */
	ttyp = &tty[devtab[device].dvminor];
	if ( ttyp->hasborder == 1 ) {
		box[0].row = ttyp->topleft.row - 1;
		box[0].col = ttyp->topleft.col - 1;
		box[1].row = ttyp->botright.row + 1;
		box[1].col = ttyp->botright.col + 1;
	
		drawborder(box[0],box[1]);
		drawname(ttyp->topleft,ttyp->botright,wnamp);
	}
	control(device,TCCLEAR);
	return(OK);
}
