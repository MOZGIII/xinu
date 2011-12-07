/* lfsflush.c - lfsflush */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  lfsflush  --  flush data and i-block for a file
 *------------------------------------------------------------------------
 */
lfsflush(flptr)
struct	flblk	*flptr;
{
	DBADDR	dba;

	if ( flptr->fl_dch == FALSE )
		return;
	dba = flptr->fl_iblk.ib_dba[flptr->fl_ipnum];
	write(flptr->fl_dev, dskbcpy(flptr->fl_buff), dba);
	flptr->fl_dch = FALSE;
	return;
}
