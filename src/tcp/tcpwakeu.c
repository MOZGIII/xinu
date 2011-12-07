/* tcpwakeup.c - tcpwakeup */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpwakeup -  wake up processes sleeping for TCP, if necessary
 *	NB: Called with tcb_mutex HELD
 *------------------------------------------------------------------------
 */
int tcpwakeup(type, ptcb)
int		type;
struct	tcb	*ptcb;
{
	int	freelen;

	if (type & READERS) {
		if (((ptcb->tcb_flags & TCBF_RDONE) ||
		    ptcb->tcb_rbcount > 0 || ptcb->tcb_rudq >= 0) &&
		    scount(ptcb->tcb_rsema) <= 0)
			xinu_signal(ptcb->tcb_rsema);
	}
	if (type & WRITERS) {
		freelen = ptcb->tcb_sbsize - ptcb->tcb_sbcount;
		if (((ptcb->tcb_flags & TCBF_SDONE) || freelen > 0) &&
		    scount(ptcb->tcb_ssema) <= 0)
			xinu_signal(ptcb->tcb_ssema);
		/* special for abort */
		if (ptcb->tcb_error && ptcb->tcb_ocsem > 0)
			xinu_signal(ptcb->tcb_ocsem);
	}
	return OK;
}
