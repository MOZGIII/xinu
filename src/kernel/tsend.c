/* tsend.c - tsend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  tsend  --  trap another process
 *------------------------------------------------------------------------
 */
SYSCALL	tsend(pid, arg)
int	pid;
int	arg;
{
	struct pentry	*pptr;
	int   		ps;

	disable(ps);
	if (isbadpid(pid) || (pptr=&proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (pid == currpid) {		/* current process has priority	*/
		pptr->phastrap = FALSE;
		if (pptr->ptfn != 0)
			(*pptr->ptfn)(arg);
		restore(ps);
		return(OK);
	}
	if (pptr->phastrap) {
		restore(ps);
		return(SYSERR);
	}
	pptr->ptarg = arg;		/* deposit argument		*/
	pptr->phastrap = TRUE;
	restore(ps);
	return(OK);
}
