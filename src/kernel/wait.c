/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(sem)
	int	sem;
{
	int	ps;
	register struct	sentry	*sptr;
	register struct	pentry	*pptr;
	int	wret;

	disable(ps);
	if (isbadsem(sem) || (sptr = &semaph[sem])->sstate == SFREE) {
		restore(ps);
        return(SYSERR);
	}
	if ( --sptr->semcnt < 0 ) {
		pptr = &proctab[currpid];
		pptr->pstate = PRWAIT;
		pptr->psem = sem;
		pptr->pwaitret = OK;
		enqueue(currpid,sptr->sqtail);
		resched();
		wret = pptr->pwaitret;
		restore(ps);
		return(wret);
	}
	restore(ps);
	return(OK);
}
