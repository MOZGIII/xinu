/*  dskchdsk.c  -  dskchdsk						*/

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>



int  dskchdsk(devptr)
struct devsw *devptr;
{
	struct	dsblk	*dsptr;
	int	i;
	int	err;


	dsptr = &dstab[i=devptr->dvminor];

	if ( dsptr->ddir == (struct dir *)NULL ) {
		kprintf("directory pointer is NULL\n");
		return(SYSERR);
	}
		
	if ( dsptr->dnfiles != 0 ) {
		kprintf("Too many open files.  dnfiles = %d\n",dsptr->dnfiles);
		return(SYSERR);
	}
		
	if ( (err=dread(dsptr->ddir,i,0)) != 0 ) {
		kprintf("Disk read error %02xH reading drive %d\n",err,i);
		dsptr->ddir = (struct dir *) NULL;
		return(SYSERR);
	}
	
	return(OK);
}
