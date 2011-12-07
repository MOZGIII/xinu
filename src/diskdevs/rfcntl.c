/* rfcntl.c - rfcntl */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfcntl  --  control the remote file server access system
 *------------------------------------------------------------------------
 */
rfcntl(devptr, func, addr, addr2)
struct	devsw	*devptr;
int	func;
char	*addr;
char	*addr2;
{
	long	junk;
	int	len;
	struct	rfblk	*rfptr;
	struct	rfmastr	*rfmptr;
	int	ret;
	int	ps;

	junk = 0L;

	rfptr = (struct rfblk *) devptr->dvioblk;
	disable(ps);
	if (rfptr->rf_state != RFUSED) {
		restore(ps);
		return(SYSERR);
	}
	rfmptr = rfptr->rf_mastptr;
	if (rfmptr->rstatus != RFMOPENED) {
		restore(ps);
		return(SYSERR);
	}
	switch (func) {
	case RFCLEAR:
		/* clear port associated with rfserver */
		control(rfmptr->rnetdev, DG_CLEAR);
		restore(ps);
		return(OK);

	/* Universal file manipulation functions */

	case FLACCESS:
		ret = rfmkpac(FS_ACCESS, addr,
			&junk, (char *)&junk, addr2, rfmptr);
		restore(ps);
		return(ret);

	case FLREMOVE:
		ret = rfmkpac(FS_REMOVE, addr, &junk, NULLSTR, 0,rfmptr);
		restore(ps);
		return(ret);

	case FLRENAME:
		len = xinu_strlen(addr2) + 1;
		ret = rfmkpac(FS_RENAME, addr, &junk, addr2, len, rfmptr);
		restore(ps);
		return(ret);

	default:
		restore(ps);
		return(SYSERR);
	}	
}
