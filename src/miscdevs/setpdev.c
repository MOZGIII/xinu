/* setpdev.c - setpdev */

#include <conf.h>
#include <kernel.h>
#include <sio.h>
#include <proc.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  setpdev  --  set a device entry in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL
setpdev(pid, siodev, dev)
int	pid;				/* process to change		*/
int	siodev, dev;			/* device descriptor to set	*/
{
	int	minor, *pdev;
	int	ps;

	disable(ps);
	if (isbadpid(pid) ||
	    proctab[pid].pstate == PRFREE ||
	    isbaddev(siodev) ||
	    (minor=devtab[siodev].dvminor) >= Nsio ||
	    siodev != sio[minor]) {
		restore(ps);
		return(SYSERR);
	}
	xinu_close( *(pdev = &proctab[pid].pdevs[minor]) );
	*pdev = (isbaddev(dev) ? BADDEV : dev);
	restore(ps);
	return(OK);
}
