/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  receive  --  wait for a message and return it
 *------------------------------------------------------------------------
 */
LSYSCALL receive()
{
	struct	pentry	*pptr;
    long msg;
	int	ps;

	disable(ps);
	pptr = &proctab[currpid];
	if (pptr->phasmsg == 0) {	/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = 0;
	restore(ps);
	return(msg);
}
