/* sioread.c - sioread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  sioread  --  read count bytes from a process-specific standard I/O dev
 *------------------------------------------------------------------------
 */
sioread(devptr, buff, count)
struct	devsw	*devptr;
char	*buff;
int	count;
{
    return(xinu_read(proctab[currpid].pdevs[devptr->dvminor],buff,count));
}
