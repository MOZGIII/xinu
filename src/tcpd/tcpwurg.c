/* tcpwurg.c - tcpwurg */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>

struct	uqe	*uqalloc();

/*------------------------------------------------------------------------
 *  tcpwurg  -  keep track of urgent data boundaries on writes
 *------------------------------------------------------------------------
 */
int tcpwurg(ptcb, sboff, len)
struct	tcb	*ptcb;
long     sboff;
int      len;
{
	struct	uqe	*puqe;

	puqe = uqalloc();
	if (puqe == (struct uqe *)SYSERR)
		return SYSERR;
	puqe->uq_seq = ptcb->tcb_suna + sboff;
	puqe->uq_len = len;
	puqe->uq_data = 0;
	if (ptcb->tcb_sudq < 0) {
		ptcb->tcb_sudq = newq(TCPUQLEN, QF_WAIT);
		ptcb->tcb_sudseq = puqe->uq_seq;
	}
	if (enq(ptcb->tcb_sudq, puqe, SUDK(ptcb, puqe->uq_seq)) < 0) {
		uqfree(puqe);
		return SYSERR;
	}
	return len;
}
