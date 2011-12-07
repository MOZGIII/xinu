/* slread.c - slread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sl.h>

/*------------------------------------------------------------------------
 *  slread - read a single packet from the serial device
 *------------------------------------------------------------------------
 */
slread(devptr, buff, len)
struct	devsw  *devptr;
char	*buff;
int	len;
{
	int	ps;
	struct slblk	*slp;
	int	plen;
	char	*p;
	
	slp = (struct slblk *)devptr->dvioblk;
	wait(slp->sl_rsem);

	disable(ps);
	slp->sl_rpid  = currpid;
    slp->sl_rbuf  = buff;
	slp->sl_rptr  = slp->sl_rbuf;
	slp->sl_rstat = SYSERR;
	slp->sl_rlen  = len;
	slp->sl_rct   = 0;
	slp->sl_resc  = FALSE;
	
	slp->sl_rawptr = slp->sl_raw;
	slp->sl_rawct = 0;

	suspend(currpid);
	
	xinu_signal(slp->sl_rsem);
	restore(ps);
		
	if ( slp->sl_rstat == SYSERR )
		return(SYSERR);
	return(slp->sl_rct);
}
