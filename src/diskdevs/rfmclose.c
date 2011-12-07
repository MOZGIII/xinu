/* rfmclose.c  -  rfmclose */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*--------------------------------------------------------------------
 * 	rfmclose   --	takes care of closing a remote master device
 *--------------------------------------------------------------------
 */
rfmclose(devptr)
struct	devsw	*devptr;
{
	struct 	rfmastr	*rfmptr;
	int	ps;

	disable(ps);
	rfmptr = (struct rfmastr *) devptr->dvioblk;
	if (rfmptr->rstatus != RFMOPENED || rfmptr->rcount > 0) {
		restore(ps);
		return(SYSERR);
	}
	/* close the datagram device */
	rfmptr->rstatus = RFMLIMBO;
	wait(rfmptr->rmutex);
	xinu_close(rfmptr->rnetdev);
	sreset(rfmptr->rmutex, 1);
	rfmptr->rnetdev = SYSERR;
	rfmptr->rstatus = RFMCLOSED;
	restore(ps);
	return(OK);
}
