/* x_goto.c  -  x_goto */

#include <conf.h>
#include <kernel.h>
#include <tty.h>


/*------------------------------------------------------------------------
 *  x_goto  --  (commands goto/home) position cursor in STDOUT device
 *------------------------------------------------------------------------
 */
COMMAND  x_goto(nargs,args)
int nargs;
char *args[];
{
	int	dev;
	char	*curp;

	if (strcmp(args[0],"home") == 0) {
		if (nargs != 1) {
			xinu_fprintf(STDERR,"Usage: %s\n",args[0]);
			return(SYSERR);
		}
		curp = "0,0";
	} else {
		if(nargs != 2) {
			xinu_fprintf(STDERR,"Usage: %s coord\n",args[0]);
			return(SYSERR);
		}
		curp = args[1];
	}
	dev = getpdev(xinu_getpid(), STDOUT);
	if (!isatty(dev)) {
		xinu_fprintf(STDERR, "%s: STDOUT is not a window\n", args[0]);
		return(SYSERR);
	}
	if (control(dev,TCCURPOS,curp) == SYSERR) {
		xinu_fprintf(STDERR,"%s: could not position cursor at %s\n",
			args[0],curp);
		return(SYSERR);
	}
	return(OK);
}
