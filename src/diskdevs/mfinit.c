/* mfinit.c - mfinit */

#include <conf.h>
#include <kernel.h>
#include <mffile.h>

/*------------------------------------------------------------------------
 *  mfinit  --  mark disk file 'device' available at system startup
 *------------------------------------------------------------------------
 */
mfinit(devptr, flag)
struct	devsw	*devptr;
int flag;
{
	struct	mfblk	*mfptr;

    if (!flag) return OK;
    
	devptr->dvioblk = (char *)(mfptr = &mftab[devptr->dvminor]);
	mfptr->mf_pid = 0;
	mfptr->mf_id = devptr->dvnum;
	return(OK);
}
