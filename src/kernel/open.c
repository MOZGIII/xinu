/* open.c - open */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  open  --  open a connection to a device/file (parms 2&3 are optional)
 *------------------------------------------------------------------------
 */
SYSCALL	xinu_open(descrp, name, mode)
int	descrp;
char	*name;
char	*mode;
{
	int		ps;
	struct	devsw	*devptr;
	int		dev;

	if ( isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	disable(ps);
	if (devptr->dvrefct > 0)
		dev = ioopen(devptr, name, mode);
	else
		dev = (*devptr->dvopen)(devptr, name, mode);
	if (!isbaddev(dev)) {
		devptr = &devtab[dev];
		if (devptr->dvrefct >= 0)
			++devptr->dvrefct;
	}
	restore(ps);
	return(dev);
}
