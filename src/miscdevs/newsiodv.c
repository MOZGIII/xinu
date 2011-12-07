/* newsiodv.c -- newsiodv */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <proc.h>
#include <disk.h>
#include <file.h>
#include <name.h>
#include <sio.h>

/*------------------------------------------------------------------------
 *  newsiodv  --  get a standard I/O device entry and associate a device
 *------------------------------------------------------------------------
 */
newsiodv(dev)
int dev;
{
	struct pentry	*pptr;
	int		siodev;
	int		ps;

	if (isbaddev(dev))
		return(SYSERR);
	disable(ps);
	pptr = &proctab[currpid];
	for (siodev=0 ; siodev < Nsio ; siodev++)
		if (pptr->pdevs[siodev] == BADDEV) { /* empty slot! */
			pptr->pdevs[siodev] = dev;
			restore(ps);
			return(sio[siodev]);	/* return the "real" sio dev */
		}
	restore(ps);
	close(dev);
	return(SYSERR);
}
