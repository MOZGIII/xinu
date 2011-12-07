/* ttyopen.c - ttyopen */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>
#include <io.h>
#include <pc.h>
LOCAL wfree();
/*------------------------------------------------------------------------
 *  ttyopen  --  open a window on a tty
 *------------------------------------------------------------------------
 */
ttyopen(devptr,bp,ap)
struct	devsw	*devptr;
char *bp;				/* border string		*/
char *ap;				/* attribute string		*/
{
    struct tty *iptr;
	int	pid;
	int	ps;
	char	cp[7];		/* output server process name		*/
	int 	i, w;
	CURSOR	boxp[2];	/* window corners from passed params	*/
	CURSOR	topl,botr;	/* topleft, bottom right of window	*/
	int	attr;		/* window attributes (color, etc.)	*/
	int	lwoproc();	/* tty server process			*/
	int	brdr;		/* true if the window has a border	*/

	if ( bp == NULL || *bp == 0 )	/* reopen the console		*/
		return(tty[0].dnum);
    if ( (brdr=lwbord(bp,boxp))==SYSERR || (attr=lwattr(ap,BW))==SYSERR ){
		return(SYSERR);
    }
	topl = boxp[0];
	botr = boxp[1];
	if ( topl.col >= G_COLS || topl.row >= G_ROWS
		|| botr.col >= G_COLS || botr.row >= G_ROWS )
		return(SYSERR);
    i = brdr ? 1 : 0;       /* offset for border        */
    if ( topl.row+i > botr.row || topl.col > botr.col )
		return(SYSERR);		/* not enough room for window	*/
	if ( brdr ) {			/* make room for the border	*/
		topl.row++;
	}
	disable(ps);
	if ( (w=wfree()) == SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	iptr = &tty[w];
	xinu_strcpy(cp,"*LWO *");
	cp[4] = '0'+w;
	if ( (pid=create(lwoproc,INITSTK,TTYOPRIO,cp,1,w)) == SYSERR ) {
		iptr->oprocnum = -1;
		iptr->wstate = LWFREE;	/* mark it as free 		*/
		restore(ps);
		return(SYSERR);
	}
	iptr->oprocnum = pid;
	ready(pid);
	iptr->hasborder = brdr;
	iptr->topleft = topl;
	iptr->botright = botr;
	iptr->attr = attr;
	iptr->rowsiz = botr.row-topl.row+1;
	iptr->colsiz = botr.col-topl.col+1;
	iptr->curcur.row = 0;
	iptr->curcur.col = 0;
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
	iptr->ikill = TRUE;			/* set line kill == @	*/
	iptr->ikillc = ESC;         /* was ATSIGN */
	iptr->oheld = FALSE;
	iptr->ostart = STRTCH;
	iptr->ostop = STOPCH;
	iptr->icursor = 0;
	iptr->ifullc = TFULLC;
	iptr->ieof = TRUE;			/* honor EOF		*/
	iptr->ieofc = EOFC;
	iptr->iintr = TRUE;			/* honor interrupt char	*/
	iptr->iintrc = INTRCH;
	iptr->iintkc = KILLCH;
	iptr->iintpid = BADPID;
	iptr->ocount = 1;
	scrollup(boxp[0],boxp[1],0,attr); /* erase the window		*/
	if ( brdr )
        drawborder(boxp[0],boxp[1],WB); /* draw the border     */
	restore(ps);
	return(iptr->dnum);
}

/*------------------------------------------------------------------------
 *  drawborder  --  draw a border around a screen window
 *------------------------------------------------------------------------
 */
drawborder(tl,br,attr)
CURSOR tl,br;
int attr;
{
	CURSOR	csr;		/* used for absolute cursor positioning	*/
	int	pcx;

	xdisable(pcx);
    csr.row=tl.row;
    csr.col=br.col;
    putcsrpos(csr,0);
    scrollup(tl,csr,1,WB);
    xrestore(pcx);
}

/*------------------------------------------------------------------------
 *  wfree  --  get a free window slot
 *------------------------------------------------------------------------
 */
LOCAL wfree()
{
	int	i;
	struct tty	*iptr;

	for ( i=1 ; i<Ntty ; i++ ) {
		iptr = &tty[i];
		if ( iptr->wstate == LWFREE ) {
			iptr->wstate = LWALLOC;
			iptr->seq++;
			return(i);
		}
	}
	return(SYSERR);
}
