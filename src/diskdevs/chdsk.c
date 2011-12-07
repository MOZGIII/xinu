/* chdsk.c - chdsk */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>


/*------------------------------------------------------------------------
 *  chdsk  -  initialize to PC-Xinu chdsk the floppy disk in drive 0
 *------------------------------------------------------------------------
 */
SYSCALL	chdsk(device)
	int	device;
{
	struct	devsw	*devptr;

	devptr = &devtab[device];
	return(	(*devptr->dvcntl)(devptr, FLCHDSK) );
}
