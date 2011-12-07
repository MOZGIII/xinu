/* x_shell.c - x_shell */

#include <conf.h>
#include <kernel.h>
#include <tty.h>

/*------------------------------------------------------------------------
 *  x_shell  --  (command shell) create a new shell in a window
 *------------------------------------------------------------------------
 */
COMMAND x_shell(nargs,args)
int nargs;
char *args[];
{
	struct	devsw	*devptr;
	int	pid;
	int	sw;
	int	shell();
	
    if (nargs <2 || nargs>4 ) {
        xinu_fprintf(STDERR,"shell: border [color] name\n");
		return(SYSERR);
	}
	/* for STDIN */
    immortal(xinu_getpid());
    if ( (sw = x_window(nargs,args)) == SYSERR ) {
		xinu_fprintf(STDERR,"shell: could not open shell window\n");
		return(SYSERR);
	}
    if ( (pid=create(shell,INITSTK,INITPRIO,"shell",3,0,NULLPTR)) == SYSERR ) {
		xinu_fprintf(STDERR,"shell: could not create new shell\n");
        xinu_close(sw);
		return(SYSERR);
	}
    /* should really have the next two lines as a control call */
    winofcur = devtab[sw].dvminor;
	send(tty[winofcur].oprocnum,TMSGOK);
    
    /* for STDOUT & STDERR */
    xinu_open(sw, NULLPTR);
    xinu_open(sw, NULLPTR);
    setpdev(pid, STDIN, sw);
	setpdev(pid, STDOUT, sw);
	setpdev(pid, STDERR, sw);
    control(sw, TCINT);
    resume(pid);
	return(OK);
}
