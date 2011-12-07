/* format.c - format */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <name.h>

/*------------------------------------------------------------------------
 *  format  -  initialize to PC-Xinu format the floppy disk in drive 0
 *------------------------------------------------------------------------
 */
SYSCALL	format(device,idnum)
	int	device;
	char	*idnum;
{
	struct	devsw	*devptr;

	devptr = &devtab[device];
	return(	(*devptr->dvcntl)(devptr, FLFORMAT, idnum) );
}
