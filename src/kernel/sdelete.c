/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL sdelete(sem)
	int	sem;
{
	int	ps;
	int	pid;
	struct	sentry	*sptr;		/* address of sem to free	*/
	int	slist;
	int	freect;			/* number of freed processes	*/

	disable(ps);
	if ( isbadsem(sem) || semaph[sem].sstate==SFREE ) {
		restore(ps);
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	sptr->sstate = SFREE;
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
