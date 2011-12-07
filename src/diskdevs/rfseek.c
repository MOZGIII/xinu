/* rfseek.c - rfseek */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfseek  --  seek to a specified position of a remote file
 *------------------------------------------------------------------------
 */
rfseek(devptr, offset)
struct	devsw	*devptr;
long	offset;
{
	struct	rfblk	*rfptr;
	int	ps;

	rfptr = (struct rfblk *)devptr->dvioblk;
	disable(ps);
	if (rfptr->rf_state != RFUSED || wait(rfptr->rf_mutex) != OK) {
		restore(ps);
		return(SYSERR);
	}
	rfptr->rf_pos = offset;
	xinu_signal(rfptr->rf_mutex);
	restore(ps);
	return(OK);
}
