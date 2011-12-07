/* immortal.c - immortal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  immortal  --  make a process immortal
 *------------------------------------------------------------------------
 */
SYSCALL immortal(pid)
int	pid;
{
	struct	pentry	*pptr;		/* receiver's proc. table addr.	*/
	int	ps;

	disable(ps);
	if (pid == 0) {			/* make me mortal */
		pptr = &proctab[currpid];
		pptr->pimmortl = FALSE;
		restore(ps);
		return(OK);
	}
	if (isbadpid(pid) || (pptr=&proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pimmortl = TRUE;
	restore(ps);
	return(OK);
}
