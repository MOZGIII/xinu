/* close.c - close */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  close  --  close a device
 *------------------------------------------------------------------------
 */
SYSCALL	xinu_close(descrp)
int descrp;
{
	int		ps;
	struct	devsw	*devptr;
	int		ret;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
	disable(ps);
	if ((devptr->dvrefct > 0) && (--devptr->dvrefct > 0))
		ret = OK;
	else
		ret = (*devptr->dvclose)(devptr);
	restore(ps);
	return(ret);
}
