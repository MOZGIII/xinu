/* ttyinit.c - ttyinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>
#include <io.h>
#include <pc.h>
#include <kbdio.h>

/*------------------------------------------------------------------------
 *  ttyinit  --  initialize buffers and modes for a tty line
 *------------------------------------------------------------------------
 */
ttyinit(devptr, flag)
struct	devsw	*devptr;
int flag;
{
	register struct	tty *iptr;
	char	*cp;
	int	pid;
	int	ttyoproc();
	int	ttyiproc();

    if (!flag) return OK;
    
    iptr = &tty[devptr->dvminor];
	devptr->dvioblk = (char *) iptr;	/* fill tty control blk	*/

	iptr->ihead = iptr->itail = 0;		/* empty input queue	*/
	iptr->isem = screate(0);		/* chars. read so far=0	*/
	iptr->icnt = 0;
	iptr->osem = screate(OBUFLEN);		/* buffer available=all	*/
	iptr->odsend = 0;			/* sends delayed so far	*/
	iptr->ohead = iptr->otail = 0;		/* output queue empty	*/
	iptr->ocnt = 0;
	iptr->ehead = iptr->etail = 0;		/* echo queue empty	*/
	iptr->ecnt = 0;
	iptr->imode = IMCOOKED;
	iptr->iecho = iptr->evis = TRUE;	/* echo console input	*/
	iptr->ierase = iptr->ieback = TRUE;	/* console honors erase	*/
	iptr->ierasec = BACKSP;			/*  using ^h		*/
	iptr->ecrlf = iptr->icrlf = TRUE;	/* map RETURN on input	*/
	iptr->ocrlf = iptr->oflow = TRUE;
    iptr->ikill = TRUE;         /* set line kill == @   */
    iptr->ikillc = ESC;      /* was ATSIGN */
	iptr->ieof = TRUE;			/* honor EOF		*/
	iptr->ieofc = EOFC;
	iptr->iintr = TRUE;			/* honor interrupts	*/
	iptr->iintrc = INTRCH;			/* interrupt char	*/
	iptr->iintkc = KILLCH;			/* process kill char	*/
	iptr->iintpid = BADPID;
	iptr->oheld = FALSE;
	iptr->ostart = STRTCH;
	iptr->ostop = STOPCH;
	iptr->icursor = 0;
	iptr->ifullc = TFULLC;
	iptr->ocount = 0;			/* no open files	*/
	iptr->curcur.row = 0;
	iptr->curcur.col = 0;
	iptr->topleft.row = 0;
	iptr->topleft.col = 0;
	iptr->botright.row = G_ROWS - 1;
	iptr->botright.col = G_COLS - 1;
	iptr->rowsiz = G_ROWS;
	iptr->colsiz = G_COLS;
	iptr->attr = 7;				/* white on black	*/

	/* now set up new tty process for this device */
	pid = create(ttyoproc,INITSTK,TTYOPRIO,"*TTYO*",0);
	if ( pid == SYSERR ) {
		kprintf("Can't create console output process\n");
		return(SYSERR);
	}
	immortal(pid);
	ready(pid);
	iptr->oprocnum = pid;
	pid = create(ttyiproc,INITSTK,TTYIPRIO,"*TTYI*",0);
	if ( pid == SYSERR ) {
		kprintf("Can't create console input process\n");
		return(SYSERR);
	}
	immortal(pid);
	ready(pid);
	iptr->wstate = pid;
	return(OK);
}
