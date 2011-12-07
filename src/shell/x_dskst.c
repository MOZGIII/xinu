/* dskst.c - x_dskst */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

#ifdef Ndsk
static struct t_flblk   {       /* file "device" control block  */
	int	fl_id;			/* file's "device id" in devtab	*/
	int	fl_dev;			/* file is on this disk device	*/
	int	fl_pid;			/* process id accessing the file*/
	int	fl_mode;		/* FLREAD, FLWRITE, or both	*/
	int	fl_ipnum;		/* current iptr in fl_iblk	*/
	long	fl_pos;			/* current file position (bytes)*/
	int	fl_ocount;		/* # of opens on this file	*/
	Bool	fl_dch;			/* has fl_buff been changed?	*/
} t_fltab[Ndf];
#endif

/*------------------------------------------------------------------------
 *  x_dskst  --  (command diskstat) print floppy disk status information
 *------------------------------------------------------------------------
 */
COMMAND x_dskst(nargs,args)
int	nargs;
char	*args[];
{
#ifdef Ndsk
    int ps,i;
	struct	t_flblk	*tabp;
	struct	flblk	*flptr;
	
	if ( nargs != 1) {
		xinu_fprintf(STDERR,"Usage: dskst\n");
		return(SYSERR);
	}
	
	xinu_printf("\nCurrent status of logical files\n");
	xinu_printf("%3s %5s %5s %5s %5s %6s %5s %7s %6s\n",
		"#","id","dev","pid","mode","ipnum","fpos","ocount","dirty");
	disable(ps);
	for (i=0 ; i<Ndf ; i++) {
		flptr = &fltab[i];
		tabp  = &t_fltab[i];
		if ( (tabp->fl_pid=flptr->fl_pid) != 0 ) {
			tabp->fl_id     = flptr->fl_id;
			tabp->fl_dev    = flptr->fl_dev;
			tabp->fl_mode   = flptr->fl_mode;
			tabp->fl_ipnum  = flptr->fl_ipnum;
			tabp->fl_pos    = flptr->fl_pos;
			tabp->fl_ocount = flptr->fl_ocount			;
			tabp->fl_dch    = flptr->fl_dch;
		}
	}
	restore(ps);
	
	for (i=0 ; i<Ndf ; i++) {
		tabp = &t_fltab[i];
		if ( tabp->fl_pid != 0 )
		xinu_printf("%3d %5d %5d %5d %5d %6d %5ld %7d %6s\n",
			i,tabp->fl_id,tabp->fl_dev,tabp->fl_pid,tabp->fl_mode,
			tabp->fl_ipnum,tabp->fl_pos,tabp->fl_ocount,
			(tabp->fl_dch)?"D":" ");
	}		
#endif
    return(OK);
}
