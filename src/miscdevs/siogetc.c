/* siogetc.c - siogetc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  siogetc  --  get one byte from a process-specific standard I/O device
 *------------------------------------------------------------------------
 */
siogetc(devptr)
struct	devsw	*devptr;
{
	return(getc(proctab[currpid].pdevs[devptr->dvminor]));
}
