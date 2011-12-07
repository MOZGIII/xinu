/* rfsend.c - rfsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfsend  --  send message to remote server and await reply
 *------------------------------------------------------------------------
 */
 
rfsend(fptr, reqlen, rplylen, dev)
struct	fphdr	*fptr;
int	reqlen;
int	rplylen;
int	dev;
{
	int	trys;
	int	ret;
	int	ps;

	disable(ps);
	control(dev, DG_CLEAR);
	for (trys=0 ; trys<RMAXTRY ; trys++) {
		if (xinu_write(dev, fptr, reqlen) == SYSERR) {
			restore(ps);
			return(SYSERR);
		}
		if ((ret=read(dev, fptr, rplylen)) !=SYSERR
		    && ret != TIMEOUT) {
			restore(ps);
			return(ret);
		}
	}
	restore(ps);
	return(SYSERR);
}
