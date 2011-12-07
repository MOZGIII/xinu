/* call.c - call */

#include <conf.h>
#include <kernel.h>
#include <dos.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  call  --  insert a procedure call into the runtime stack of a process
 *------------------------------------------------------------------------
 */
SYSCALL
call(pid, fn)
int	pid;
int	(*fn)();
{
	int		*sp, *sp1;
	struct pentry	*pptr;
	int		ps;

	if (pid == currpid)
		return((*fn)());
	disable(ps);
	if (isbadpid(pid) || (pptr=&proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	sp = (int *)pptr->pregs;
	sp1 = sp - 2;
	if (sp1 >= sp) {		/* stack overflow */
		restore(ps);
		return(SYSERR);
	}
	pptr->pregs = (char *)sp1;
	*sp1++ = *sp++;			/* DI */
	*sp1++ = *sp++;			/* SI */
	*sp1++ = *sp++;			/* FLAGS */
	*sp1++ = *sp++;			/* BP */
	*--sp = FP_SEG(fn);
	*--sp = FP_OFF(fn);
	restore(ps);
	return(OK);
}
