/* init.c - init */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  init  --  initialize a device
 *------------------------------------------------------------------------
 */
init(descrp, flag)
int descrp, flag;
{
	struct	devsw	*devptr;

	if (isbaddev(descrp) )
		return(SYSERR);
	devptr = &devtab[descrp];
    return( (*devptr->dvinit)(devptr, flag) );
}
