/* rfinit.c - rfinit */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rfinit  --  initialize remote file pseudo devices
 *------------------------------------------------------------------------
 */
rfinit(devptr, flag)
struct	devsw	*devptr;
int flag;
{
	struct	rfblk	*rfptr;
    
    if (!flag) return OK;
    
	devptr->dvioblk = (char *) (rfptr = &rftab[devptr->dvminor]);
	rfptr->rf_dnum = devptr->dvnum;
	rfptr->rf_name[0] = NULLCH;
	rfptr->rf_state = RFFREE;
	rfptr->rf_mutex = screate(1);
	rfptr->rf_pos = 0L;
    return OK;
}

struct	rfblk	rftab[Nrf];
