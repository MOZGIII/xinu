/* lfopen.c  -  lfopen */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  lfopen  --  accept an open call on an already open logical file
 *------------------------------------------------------------------------
 */
SYSCALL lfopen(devptr)
struct devsw *devptr;
{
	int	ps;
	struct	flblk	*flptr;
 	
	disable(ps);
#ifdef Ndf
    flptr = (struct flblk *) &fltab[devptr->dvminor];
	if ( flptr->fl_pid == 0 ) {
		restore(ps);
		return(SYSERR);
	}
	flptr->fl_ocount++;
#endif
    restore(ps);
	return(devptr->dvnum);
}
