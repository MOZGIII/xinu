/* lfclose.c - lfclose */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  lfclose  --  close a file by flushing output and freeing device slot
 *------------------------------------------------------------------------
 */
lfclose(devptr)
struct	devsw	*devptr;
{
	struct	dsblk	*dsptr;
	struct	dir	*dirptr;
	struct	flblk	*flptr;
	int	diskdev;
	int	ps;

	disable(ps);
	flptr   = (struct flblk *) devptr->dvioblk;
	diskdev = flptr->fl_dev;
	dsptr   = (struct dsblk *)devtab[diskdev].dvioblk;
 	if ( --flptr->fl_ocount != 0) {
 		restore(ps);
		return(OK);
	}
	dirptr = (struct dir *) dsptr->ddir;
	if ( (flptr->fl_mode & FLWRITE) && flptr->fl_dch)
		lfsflush(flptr);
	flptr->fl_pid = 0;
	dsptr->dnfiles--;
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	restore(ps);
	return(OK);
}
