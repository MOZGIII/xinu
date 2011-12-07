/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL sreset(sem,count)
	int	sem;
	int	count;
{
	struct	sentry	*sptr;
	int	ps;
	int	pid;
	int	slist;
	int	freect;			/* number of freed processes	*/

	disable(ps);
	if ( isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE ) {
		restore(ps);
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	sptr->semcnt = count;
	freect = 0;
	while ( (pid=getfirst(slist)) != EMPTY ) {
		proctab[pid].pwaitret = 0;
		ready(pid);
		freect++;
	}
	resched();
	restore(ps);
	return(freect);
}
