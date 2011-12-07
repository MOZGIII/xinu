/* lfinit.c - lfinit */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

#ifndef	Ndf
#define	Ndf	1
#endif
struct	flblk	fltab[Ndf];

/*------------------------------------------------------------------------
 *  lfinit  --  mark disk file 'device' available at system startup
 *------------------------------------------------------------------------
 */
lfinit(devptr, flag)
struct	devsw	*devptr;
int flag;
{
	struct	flblk	*flptr;

    if (!flag) return OK;
    
	devptr->dvioblk = (char *)(flptr = &fltab[devptr->dvminor]);
	flptr->fl_pid = 0;
	flptr->fl_id = devptr->dvnum;
	flptr->fl_ocount = 0;
	return(OK);
}
