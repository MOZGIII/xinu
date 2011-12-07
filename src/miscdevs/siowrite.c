/* siowrite.c - siowrite */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  siowrite  --  write count bytes to process-specific standard I/O dev
 *------------------------------------------------------------------------
 */
siowrite(devptr, buff, count)
struct	devsw	*devptr;
char	*buff;
int	count;
{
	return(xinu_write(proctab[currpid].pdevs[devptr->dvminor],buff,count));
}
