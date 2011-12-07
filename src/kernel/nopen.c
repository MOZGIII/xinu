/* nopen.c - nopen */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  nopen  --  open a connection to a device/file without reference counts
 *------------------------------------------------------------------------
 */
SYSCALL	nopen(descrp, name, mode)
int	descrp;
char	*name;
char	*mode;
{
	struct	devsw	*devptr;
	int		dev;
	int		ps;
	if ( isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	disable(ps);
	dev = (*devptr->dvopen)(devptr, name, mode);
	restore(ps);
    return(dev);
}
