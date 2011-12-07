/* getpdev.c - getpdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sio.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  getpdev  --  return the 'real' device the sio device is connected to
 *------------------------------------------------------------------------
 */
SYSCALL
getpdev(pid, siodev)
int	pid;
int	siodev;
{
	int	minor;
	int	dev;
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
	dev = proctab[pid].pdevs[minor];
	restore(ps);
	return(dev);
}
