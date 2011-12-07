/* slcntl.c  -  slcntl */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <sl.h>


/*------------------------------------------------------------------------
 *  slcntl  --  process a control call to the serial line device
 *------------------------------------------------------------------------
 */
int
slcntl(devptr, func, arg1, arg2)
struct	devsw	*devptr;
int	func;
char	*arg1, *arg2;
{
	int	ps;
	int	len;
	struct	slblk *slp;
	char	*cp;
#ifdef Nsl    
	slp = &sltab[devptr->dvminor];
	
	disable(ps);
	switch (func ) {
	case SL_PRNTRAW:
		wait(slp->sl_rsem);
		len = slp->sl_rawct;
		if ( (cp=xmalloc(len)) == NULL ) {
			restore(ps);
			return(SYSERR);
		}
		blkcopy(cp,slp->sl_raw,len);
		xinu_signal(slp->sl_rsem);
		restore(ps);
		*(char **)arg1 = cp;
		return(len);
	}
#endif    
	restore(ps);
	return(OK);
}
