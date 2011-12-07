/* dsinit.c - dsinit */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <mark.h>
#include <bufpool.h>
#include <dskio.h>

#ifdef	Ndsk
struct	dsblk	dstab[Ndsk];
#endif

int	dskdbp, dskrbp;

/*------------------------------------------------------------------------
 *  dsinit  --  initialize disk drive device
 *------------------------------------------------------------------------
 */
int dsinit(devptr, flag)
struct devsw *devptr;
int flag;
{
	struct	dsblk	*dsptr;
	int	pid;
	char	cp[8];
	int	dsinter();
	int	i;
	int	err;

    if (!flag) return OK;
    
	i = devptr->dvminor;
	devptr->dvioblk = (char *)(dsptr = &dstab[i]);
	dsptr->dsprocnum = -1;		/* impossible process no. for now */
	dsptr->dreqlst = DRNULL;
	dsptr->dnum    = devptr->dvnum;
	dsptr->dnfiles = 0;
	if ( (dsptr->ddir=(struct dir *)getbuf(dskdbp)) == (struct dir *)NULL)
		return(SYSERR);
    if ( (err=dread(dsptr->ddir,i,0)) != 0 ) {
		kprintf("Disk read error %02xH reading drive %d\n",err,i);
		freebuf(dsptr->ddir);
		dsptr->ddir = (struct dir *) NULL;
        return(OK);
    }
    dsptr->dibsem  = screate(1);
    dsptr->dflsem  = screate(1);
    dsptr->ddirsem = screate(1);
    xinu_strcpy(cp,"*DISK *");
	cp[5] = '0'+i;
    pid = create(dsinter,INITSTK,DSPRIO,cp,3,dsptr,i);
    dsptr->dsprocnum = pid;
	immortal(pid);
	ready(pid);	
	return(OK);
}
