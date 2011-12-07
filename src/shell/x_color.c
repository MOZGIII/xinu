/* x_color.c - x_color */

#include <conf.h>
#include <kernel.h>
#include <tty.h>


/*------------------------------------------------------------------------
 *  x_color  --  (command color) change the color attribute of STDOUT
 *------------------------------------------------------------------------
 */
int x_color(nargs,args)
int nargs;
char *args[];
{
	char	*cp;
	struct	devsw	*devptr;
	int e;
	if (nargs > 2) {
		xinu_fprintf(STDERR,"Usage: color [attr]\n");
		return(SYSERR);
	}
	if (!isatty(getpdev(xinu_getpid(), STDOUT))) {
		xinu_fprintf(STDERR, "color: standard output is not a window\n");
		return(SYSERR);
	}
	cp = "7/0";			/* white on black */
	if (nargs == 2)
		cp = args[1];
	e = control(STDOUT, TCATTR, cp);
	control(STDOUT, TCCLEAR);
	return e;
}
