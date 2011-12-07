/* x_kill.c - x_kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <shell.h>

/*------------------------------------------------------------------------
 *  x_kill  --  (builtin command kill) terminate a process
 *------------------------------------------------------------------------
 */
BUILTIN	x_kill(nargs, args)
int	nargs;
char	*args[];
{
	int	pid;
	int	r;

	if (nargs != 2) {
		xinu_fprintf(STDERR, "Usage: kill pid\n");
		return(SYSERR);
	}
	if ( (pid=atoi(args[1])) == xinu_getpid() )
		xinu_fprintf(STDERR,"Shell killed\n");
    if (isbadpid(pid) || (proctab[pid].pstate == PRFREE)) {
        xinu_fprintf(STDERR, "kill: bad pid %d\n", pid);
        return SYSERR;
    }        
    if (proctab[pid].pimmortl)
        xinu_fprintf(STDERR,"kill: process %d is immortal\n",pid);
	if ( (r=kill(pid)) == SYSERR )
		xinu_fprintf(STDERR,"kill: could not kill %s\n",args[1]);
	return(r);
}
