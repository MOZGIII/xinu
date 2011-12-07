/* sioinit.c -- sioinit */

#include <conf.h>
#include <kernel.h>
#include <sio.h>

int sio[Nsio];

/*------------------------------------------------------------------------
 *  sioinit  --  initialize the sio device cross reference table
 *------------------------------------------------------------------------
 */
sioinit(devptr, flag)
struct devsw *devptr;
int flag;
{
    if (!flag) return OK;
	sio[devptr->dvminor] = devptr->dvnum;
    return OK;
}
