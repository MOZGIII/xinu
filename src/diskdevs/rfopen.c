/* rfopen.c - rfopen */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfopen  --  open a remote file
 *------------------------------------------------------------------------
 */
rfopen(devptr, name, mode)
struct	devsw	*devptr;
char	*name;
char	*mode;
{
	struct	rfblk	*rfptr;
	struct	rfmastr	*rfmptr;
	int	i;
	int	mbits;
	int	dev;
	int	ps;

	disable(ps);
	rfmptr = (struct rfmastr *) devptr->dvioblk;
	if (rfmptr->rstatus != RFMOPENED) {
		restore(ps);
		return(SYSERR);
	}
	if (xinu_strlen(name) > RNAMLEN
	    || (mbits=dfckmd(mode)) == SYSERR
	    || (i=rfalloc()) == SYSERR) {
		restore(ps);
		return(SYSERR);
	}
	rfptr = &rftab[i];
	dev = rfptr->rf_dnum;
	xinu_strcpy(rfptr->rf_name, name);
	rfptr->rf_mode = mbits;
	rfptr->rf_pos = 0L;
	rfptr->rf_mastptr = rfmptr;
	rfmptr->rcount++;

	/* send remote file open request */

	if ( rfio(&devtab[dev], FS_OPEN, NULLSTR, mbits) == SYSERR ) {
		rfptr->rf_state = RFFREE;
		restore(ps);
		return(SYSERR);
	}
	restore(ps);
	return(dev);
}
