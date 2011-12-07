/* dskname.c - dskname */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <mark.h>
#include <bufpool.h>
#include <file.h>
#include <io.h>


/*------------------------------------------------------------------------
 *  dskname  --  rename a directory file
 *------------------------------------------------------------------------
 */
int dskname (diskdev,fname,tname)
int	diskdev;
char	*fname;
char	*tname;
{
	struct	fdes	*fdptr, *fdptr2;
	struct	dir	*dirptr;
	int	i,pos;
	int	sem;

	if (isbaddev(diskdev) || xinu_strlen(tname) >= FDNLEN )
		return(SYSERR);
	if ( (dirptr=dsdirec(diskdev)) == ( struct dir *) NULL )
		return(SYSERR);
	sem = ( (struct dsblk *)devtab[diskdev].dvioblk)->ddirsem;
	wait(sem);
	for (pos=-1, i=0 ; i<dirptr->d_nfiles ; i++) {
		fdptr = &dirptr->d_files[i];
		if ( strcmp(fname,fdptr->fdname) == 0) {
			pos = i;
		} else if ( strcmp(tname,fdptr->fdname) == 0) {
			xinu_signal(sem);
			return(SYSERR);
		}
	}
	if (pos >=0) {
		xinu_strcpy(dirptr->d_files[pos].fdname, tname);
		write(diskdev, dskbcpy(dirptr), DIRBLK);
		xinu_signal(sem);
		return(OK);
	}
	xinu_signal(sem);
	return(SYSERR);
}
