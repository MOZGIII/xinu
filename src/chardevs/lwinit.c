/* lwinit.c - lwinit */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <pc.h>

/*------------------------------------------------------------------------
 *  lwinit  --  initialize console window
 *------------------------------------------------------------------------
 */
lwinit(devptr, flag)
	struct	devsw	*devptr;
    int flag;
{
	register struct	tty *iptr;

    if (!flag) {
        lwclose(devptr);
        return OK;
    }

	iptr = &tty[devptr->dvminor];
	iptr->dnum = devptr->dvnum;
	iptr->oprocnum = -1;			/* no output process	*/
	iptr->wstate = LWFREE;			/* window is free	*/
	iptr->seq = 0;				/* init sequence no.	*/
	iptr->ocount = 0;			/* zero times opened	*/
	devptr->dvioblk = (char *) iptr;	/* fill tty control blk	*/
    return OK;
}
