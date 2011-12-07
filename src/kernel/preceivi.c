/* preceivi.c - preceivi */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  preceivi  --  non-blocking receive a message from a port
 *------------------------------------------------------------------------
 */
char *preceivi(portid)
int portid;
{
	int	ps;
	struct	pt	*ptptr;
	int	sem;
	char	*msg;
	struct	ptnode	*nxtnode;

	disable(ps);
	if ( isbadport(portid) ||
#ifdef MEMMARK
		unmarked(ptmark) ||
#endif
		(ptptr=&ports[portid])->ptstate != PTALLOC ) {
		restore(ps);
        return(NULLPTR);
	}

	sem = ptptr->ptrsem;
	if ( scount(sem) <= 0 || wait(sem) != OK ) {
		restore(ps);
        return(NULLPTR);
	}

	/* dequeue the first message that is waiting in the port */

	nxtnode = ptptr->pthead;
	msg = nxtnode->ptmsg;
	if (ptptr->pthead == ptptr->pttail)	/* delete the last item */
		ptptr->pthead = ptptr->pttail = (struct ptnode *)NULL;
	else
		ptptr->pthead = nxtnode->ptnext;
	nxtnode->ptnext = ptfree;		/* return to free list */
	ptfree = nxtnode;
	ptptr->ptct--;
	xinu_signal(ptptr->ptssem);
	restore(ps);
	return(msg);
}
