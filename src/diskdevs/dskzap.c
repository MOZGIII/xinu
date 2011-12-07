/* dskzap.c - dskzap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <mark.h>
#include <bufpool.h>
#include <file.h>
#include <io.h>


/*------------------------------------------------------------------------
 *  dskzap  --  remove a file from a directory
 *------------------------------------------------------------------------
 */
int dskzap (diskdev,fname)
int	diskdev;
char	*fname;
{
	struct	fdes	*fdptr, *fdptr2;
	struct	dir	*dirptr;
	int	i, j, sem;
	IBADDR	ilist;

	if (isbaddev(diskdev))
		return(SYSERR);
		
	if ( (dirptr=dsdirec(diskdev)) == ( struct dir *) NULL )
		return(SYSERR);
		
	sem = ( (struct dsblk *)devtab[diskdev].dvioblk)->ddirsem;
	wait(sem);
	for (i=0 ; i<dirptr->d_nfiles ; i++) {
		fdptr = &dirptr->d_files[i];
		if ( strcmp(fname, fdptr->fdname) == 0) {
			ilist = fdptr->fdiba;
			dirptr->d_nfiles--;
			fdptr2 = &dirptr->d_files[i+1];
			for (j=i ; j<dirptr->d_nfiles ; j++)
				*fdptr++ = *fdptr2++;
			write(diskdev, dskbcpy(dirptr), DIRBLK);
			xinu_signal(sem);
			iblfree(diskdev, ilist);
			return(OK);
		}
	}
	xinu_signal(sem);
	return(SYSERR);
}
