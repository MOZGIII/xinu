/* psend.c - psend */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  psend  --  send a message to a port by enqueuing it
 *------------------------------------------------------------------------
 */

SYSCALL psend(portid, msg)
int portid;
char *msg;
{
	int	ps;
	struct	pt	*ptptr;
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

	/* wait for space and verify that the port is still allocated */

	if (wait(ptptr->ptssem) != OK || ptptr->ptstate != PTALLOC ) {
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
