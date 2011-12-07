/* siocntl.c - siocntl */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  siocntl  --  control a process-specific standard I/O device
 *------------------------------------------------------------------------
 */
siocntl(devptr, func, addr, addr2)
struct	devsw	*devptr;
int	func;
char	*addr, *addr2;
{
	return(control(proctab[currpid].pdevs[devptr->dvminor],
		    func, addr, addr2));
}
