/* rfio.c - rfio */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfio  --  perform input or output using remote file server
 *------------------------------------------------------------------------
 */
rfio(devptr, rop, buff, len)
struct	devsw	*devptr;
int	rop;
char	*buff;
int	len;
{
	struct	rfblk	*rfptr;
	int	retcode;
	int	ps;

	rfptr = (struct rfblk *)devptr->dvioblk;
	disable(ps);
	if (len < 0 || rfptr->rf_state != RFUSED) {
		restore(ps);
		return(SYSERR);
	}
	if (wait(rfptr->rf_mutex) != OK) {
		restore(ps);
		return(SYSERR);
	}
	retcode=rfmkpac(rop, rfptr->rf_name, &rfptr->rf_pos, 
			buff, len, rfptr->rf_mastptr);
	xinu_signal(rfptr->rf_mutex);
	restore(ps);
	return(retcode);
}
