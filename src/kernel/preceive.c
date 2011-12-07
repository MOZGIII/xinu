/* preceive.c - preceive */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  preceive  --  receive a message from a port, blocking if port empty
 *------------------------------------------------------------------------
 */
char *preceive(portid)
int portid;
{
	int	ps;
	struct	pt	*ptptr;
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

	/* wait for a message and verify that the port is still allocated */

	if ( wait(ptptr->ptrsem) != OK || ptptr->ptstate  != PTALLOC ) {
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
