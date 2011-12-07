/* psendi.c - psendi */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  psendi  --  non-blocking send message to a port by enqueuing it
 *------------------------------------------------------------------------
 */

SYSCALL psendi(portid, msg)
int portid;
char *msg;
{
	int	ps;
	struct	pt	*ptptr;
	int	sem;
	struct	ptnode	*freenode;

	disable(ps);
	if ( isbadport(portid) ||
#ifdef	MEMMARK
		unmarked(ptmark) ||
#endif
		(ptptr=&ports[portid])->ptstate != PTALLOC ) {
		restore(ps);
		return(SYSERR);
	}

	sem = ptptr->ptssem;
    if ( scount(sem) <= 0 || wait(sem) != OK) {
			restore(ps);
			return(SYSERR);
	}
	if ( ptfree == (struct ptnode *)NULL )
		panic("psend - out of nodes");
	freenode = ptfree;
	ptfree = freenode->ptnext;
	freenode->ptnext = (struct ptnode *)NULL;
	freenode->ptmsg = msg;
	if ( ptptr->pttail == (struct ptnode *)NULL )	/* empty queue */
		ptptr->pttail = ptptr->pthead = freenode;
	else {
		(ptptr->pttail)->ptnext = freenode;
		ptptr->pttail = freenode;
	}
	ptptr->ptct++;
	xinu_signal(ptptr->ptrsem);
	restore(ps);
	return(OK);
}
