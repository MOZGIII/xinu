/* sioputc.c - sioputc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  sioputc  --  write one byte to a process-specific standard I/O device
 *------------------------------------------------------------------------
 */
sioputc(devptr, ch)
struct	devsw	*devptr;
char	ch;
{
	return(putc(proctab[currpid].pdevs[devptr->dvminor], ch));
}
