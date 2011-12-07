/* lwopen.c - lwopen */

#include <conf.h>
#include <kernel.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  lwopen  --  accept an open call on an already open logical window
 *------------------------------------------------------------------------
 */
int lwopen(devptr)
struct devsw *devptr;
{
	int	ps;
	struct	tty	*ttyp;
 	
	disable(ps);
	ttyp = &tty[devptr->dvminor];
	if ( ttyp->wstate != LWALLOC ) {
		restore(ps);
		return(SYSERR);
	}
	ttyp->ocount++;
    restore(ps);
	return(devptr->dvnum);
}
