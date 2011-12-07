/* dir.c - dir */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <sleep.h>
#include <io.h>
#include <disk.h>
#include <file.h>
#include <name.h>
static dskdir(int diskdev);
/*------------------------------------------------------------------------
 *  x_dir  --  get a disk directory from a PC-Xinu formatted disk
 *------------------------------------------------------------------------
 */
COMMAND x_dir(argc,argv)
int	argc;
char	**argv;
{
	int     dskdir();
	int	device;
	char	*drvname;
	
    if (argc > 1) {
        xinu_fprintf(STDERR,"Usage: dir\n");
		return(SYSERR);
	}
    drvname = "ds0:";
	if ((device=nammapd(drvname)) == SYSERR) {
		xinu_fprintf(STDERR,"dir: unknown device %s\n",drvname);
		return(SYSERR);
	}
	if (dskdir(device) == SYSERR) {
		xinu_fprintf(STDERR,"dir: error while reading %s\n",drvname);
		return(SYSERR);
	}
	return(OK);
}

/*------------------------------------------------------------------------
 *  dskdir  --  print file system directory in readable form
 *------------------------------------------------------------------------
 */
static dskdir(diskdev)
int	diskdev;
{
	DBADDR	firstdb, lastdb;
	struct	dir	*dirptr;
	struct	fdes	*fdptr;
	int	i, l;
	int	sem;

	if (isbaddev(diskdev))
		return(SYSERR);		
	if ( (dirptr=dsdirec(diskdev)) == ( struct dir *) NULL )
		return(SYSERR);
	sem = ( (struct dsblk *)devtab[diskdev].dvioblk)->ddirsem;
	immortal(xinu_getpid());
	wait(sem);
	if (dirptr->d_nfiles < 0 || dirptr->d_nfiles > NFDES
			|| dirptr->d_iblks < 0) {
		xinu_printf("\n\nError: invalid directory format");
		xinu_printf(" (id=%d , #files=%d)\n",dirptr->d_id,
				dirptr->d_nfiles);
		xinu_signal(sem);
		return(SYSERR);
	}
	
	xinu_printf("\n***Directory of %s     %d files      ",
		devtab[diskdev].dvnam,dirptr->d_nfiles);
	xinu_printf("(free lists: i=%d/%d, d=%d, id=%d)\n",
		dirptr->d_filst, dirptr->d_iblks,
		dirptr->d_fblst, dirptr->d_id);
	if (dirptr->d_nfiles == 0) {
		xinu_signal(sem);
		return(OK);
	}
	
	xinu_printf("\nFile        Name       Size   First iblock\n");
	for (i=0 ; i<dirptr->d_nfiles ; i++) {
		if ( recvclr() == TMSGKILL )
			break;
		fdptr = &(dirptr->d_files[i]);
		xinu_printf("%3d       ",i+1);
		if ((l=xinu_strlen(fdptr->fdname)) <= 0 || l >= FDNLEN)
			xinu_printf("*ERROR*\n");
		else xinu_printf("%-10s%7D%11d\n",fdptr->fdname,
				fdptr->fdlen, fdptr->fdiba);
	}
	xinu_signal(sem);
	return(OK);
}
