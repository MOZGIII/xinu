/* tcprhskip.c - tcprhskip */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  tcprhskip  -  skip a read hole in a TCP sequence
 *------------------------------------------------------------------------
 */
struct uqe *tcprhskip(ptcb, puqe, seq)
struct	tcb	*ptcb;
struct	uqe	*puqe;
tcpseq		seq;
{
	if (seq < puqe->uq_seq + puqe->uq_len) {
		ptcb->tcb_rbcount -= puqe->uq_len;
		ptcb->tcb_rbstart += puqe->uq_len;
		ptcb->tcb_rbstart %= ptcb->tcb_rbsize;
	}
	uqfree(puqe);
	puqe = (struct uqe *)deq(ptcb->tcb_ruhq);
	if (puqe == 0) {
		freeq(ptcb->tcb_ruhq);
		ptcb->tcb_ruhq = -1;
	}
	return puqe;
}
