/* x_window.c - x_window */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <tty.h>
#include <name.h>
LOCAL	wopen(int nargs, char *args[]);
LOCAL	wclose(int nargs, char *args[]);
/*------------------------------------------------------------------------
 *  x_window  --  (command window) create or close a window
 *------------------------------------------------------------------------
 */
COMMAND  x_window(nargs,args)
int	nargs;
char	*args[];
{
	if ( nargs>2 && strcmp(args[1],"close") == 0 )
		return(wclose(nargs,args));
	else return(wopen(nargs,args));
}

/*------------------------------------------------------------------------
 *  wopen  -  open and mount a new window dev
 *------------------------------------------------------------------------
 */
LOCAL	wopen(nargs, args)
int	nargs;
char	*args[];
{
	int	dev;
	char	*bordp;
	char	*attrp;
	int	open();
	int	mount();
	struct	tty  *ttyp;
	
	if ( nargs<3 || nargs>4) {
		xinu_fprintf(STDERR,"Usage: window border [attr] name\n");
		xinu_fprintf(STDERR,"       window close name\n");
		return(SYSERR);
	}
	bordp = args[1];
	if ( nargs == 4 ) {
		attrp = args[2];
		args++;
	} else
		attrp = "";
    if ( (dev = xinu_open(CONSOLE,bordp,attrp)) == SYSERR) {
		xinu_fprintf(STDERR,"window: could not create new window");
		return(SYSERR);
	}
	if ( mount(args[2],dev,"") == SYSERR ) {
		xinu_fprintf(STDERR,"window: could not mount new window\n");
		return(SYSERR);
	}
	
	ttyp = &tty[devtab[dev].dvminor];
	if ( ttyp->hasborder == 1 )
		drawname(ttyp->topleft,ttyp->botright,args[2]);
	
	/* The value 9 in the following statement is taken from the
	 * maximum length of a dev name (see conf.h) which does
	 * not have a defined constant.  Someday we hope it will.
	 */
	if ( xinu_strlen(args[2]) <= 9 ) 
		xinu_strcpy(devtab[dev].dvnam,args[2]);
    return(dev);
}

/*------------------------------------------------------------------------
 *  drawname  --  put the name of the window in the top border
 *------------------------------------------------------------------------
 */
drawname(tl,br,wnamp)
CURSOR	tl, br;
char	*wnamp;
{
	CURSOR	csr;
	int	wwidth;		/* width of the window			*/
    int ps;

    if ((wnamp == NULL) || (*wnamp==0))
		return;

    disable(ps);

	wwidth  = br.col - tl.col;
	csr.row = tl.row - 1;
	csr.col = tl.col + (wwidth - xinu_strlen(wnamp) - 2) / 2;
	putcsrpos(csr,0);
	putchr(' ',1,0);
	for ( csr.col++ ; *wnamp != 0; csr.col++ ) {
		putcsrpos(csr,0);
		putchr(*wnamp++,1,0);
	}
	putcsrpos(csr,0);
	putchr(' ',1,0);
    restore(ps);
}

/*------------------------------------------------------------------------
 *  wclose  -  close and unmount a window dev
 *------------------------------------------------------------------------
 */
LOCAL	wclose(nargs, args)
int	nargs;
char	*args[];
{
	int	dev;

	if ( nargs != 3 ) {
		xinu_fprintf(STDERR,"Usage: window close name\n");
		return(SYSERR);
	}
	if ((dev=nammapd(args[2])) == SYSERR) {
		xinu_fprintf(STDERR,"window: unknown device %s\n", args[2]);
		return(SYSERR);
	}
	if (!isatty(dev)) {
		xinu_fprintf(STDERR,"window: %s is not a tty\n", args[2]);
		return(SYSERR);
	}
	if (xinu_close(dev) == SYSERR) {
		xinu_fprintf(STDERR,"window: could not close %s (%d)\n",
		    args[2], dev);
		return(SYSERR);
	}
	if (unmount(args[2]) == SYSERR) {
		xinu_fprintf(STDERR,"window: could not unmount %s\n",args[2]);
		return(SYSERR);
	}
	xinu_strcpy(devtab[dev].dvnam,"");
	return(OK);
}
