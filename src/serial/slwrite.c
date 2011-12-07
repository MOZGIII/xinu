/* slwrite.c - slwrite */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sl.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  slwrite - write a single packet to the serial line device
 *------------------------------------------------------------------------
 */
slwrite(devptr, buff, len)
struct	devsw	*devptr;
char	*buff;
int     len;            
{
	struct	slblk	*sptr;

	sptr = (struct slblk *) devptr->dvioblk;
	wait(sptr->sl_wsem);
	
	sptr->sl_wbuf = buff;
    sptr->sl_wlen = len;

	xinu_signal(sptr->sl_wstsem);
    return(len);
}
