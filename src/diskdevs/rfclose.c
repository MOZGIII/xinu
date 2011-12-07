/* rfclose.c - rfclose */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfclose  --  close a remote file by deallocating pseudo device
 *------------------------------------------------------------------------
 */
rfclose(devptr)
struct	devsw	*devptr;
{
	struct	rfblk	*rfptr;
	struct	rfmastr	*rfmptr;
	long	junk;		/* argument to rfmkpac; not really used	*/
	int	ps;

	rfptr = (struct rfblk *)devptr->dvioblk;
	disable(ps);
	if (rfptr->rf_state != RFUSED) {
		restore(ps);
		return(SYSERR);
	}
	rfptr->rf_state = RFLIMBO;
	wait(rfptr->rf_mutex);
	rfmptr = rfptr->rf_mastptr;
	junk = 0L; /* 0L is long zero constant */
	rfmkpac(FS_CLOSE, rfptr->rf_name,
			&junk,(char *)&junk, 0, rfmptr);
	rfmptr->rcount--;
	sreset(rfptr->rf_mutex, 1);
	rfptr->rf_state = RFFREE;
	restore(ps);
	return(OK);
}
