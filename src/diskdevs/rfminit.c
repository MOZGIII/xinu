/* rfminit.c - rfminit */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*-----------------------------------------------------------------------
 *	rfminit  --  initialize the remote file master devices
 *-----------------------------------------------------------------------
 */
rfminit(devptr, flag)
struct 	devsw	*devptr;
int flag;
{
	struct	rfmastr	*rfmptr;

    if (!flag) return OK;
        
    devptr->dvioblk = (char *) (rfmptr = Rf);
	rfmptr->rdev = devptr->dvnum;
	rfmptr->rcount = 0;
	rfmptr->rstatus = RFMCLOSED;
	rfmptr->rmutex = screate(1);
    return OK;
}
struct rfmastr Rf[Nrfm];
