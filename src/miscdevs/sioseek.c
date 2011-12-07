/* sioseek.c - sioseek */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  sioseek  --  position a process-specific standard I/O device
 *------------------------------------------------------------------------
 */
sioseek(devptr, pos)
struct	devsw	*devptr;
long	pos;
{
	return(seek(proctab[currpid].pdevs[devptr->dvminor], pos));
}
