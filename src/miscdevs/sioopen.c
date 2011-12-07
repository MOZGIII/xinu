/* sioopen.c - sioopen */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <io.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  sioopen  --  open a process-specific standard I/O device
 *------------------------------------------------------------------------
 */
sioopen(devptr, name, mode)
struct	devsw	*devptr;
char	*name;
char	*mode;
{
	int	dev;

	/* Reopen the device by opening the named device and	*/
	/* closing the current device				*/

    if (name == NULLPTR || *name == NULLCH)
		dev = getpdev(currpid, devptr->dvnum);
	else
		dev = NAMESPACE;
	if (isbaddev(dev) || (dev=open(dev, name, mode)) == SYSERR)
		return(SYSERR);
	setpdev(currpid, devptr->dvnum, dev);
	return(devptr->dvnum);
}
