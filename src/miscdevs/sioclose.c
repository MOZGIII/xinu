/* sioclose.c - sioclose */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  sioclose  --  close a process-specific standard I/O device
 *------------------------------------------------------------------------
 */
sioclose(devptr)
struct	devsw	*devptr;
{
	int	dev, *pdev;

	dev = *(pdev = &proctab[currpid].pdevs[devptr->dvminor]);
	*pdev = BADDEV;
	return(xinu_close(dev));
}
