/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL xinu_signal(sem)
register int	sem;
{
	register struct	sentry	*sptr;
	int	ps;

	disable(ps);
	if (isbadsem(sem) || (sptr = &semaph[sem])->sstate == SFREE) {
		restore(ps);
		return(SYSERR);
	}
	if ( sptr->semcnt++ < 0 ) {
		ready(getfirst(sptr->sqhead));
		resched();
	}
	restore(ps);
	return(OK);
}
