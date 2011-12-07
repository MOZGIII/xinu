/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(pid, msg)
int	pid;
long msg;
{
	struct	pentry	*pptr;		/* receiver's proc. table addr.	*/
	int   	ps;

	disable(ps);
	if (isbadpid(pid) || ( (pptr= &proctab[pid])->pstate == PRFREE)
	   || pptr->phasmsg) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pmsg = msg;		/* deposit message		*/
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV) {	/* if receiver waits, start it	*/
		ready(pid);
		resched();
	}
	else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid);
		resched();
	}
	restore(ps);
	return(OK);
}
