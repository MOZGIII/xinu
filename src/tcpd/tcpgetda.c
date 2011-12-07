/* tcpgetdata.c - tcpgetdata */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  tcpgetdata  -  copy data from a TCP receive buffer to a user buffer
 *------------------------------------------------------------------------
 */
int tcpgetdata(ptcb, pch, len)
struct	tcb	*ptcb;
char		*pch;
int		len;
{
	struct	uqe	*puqe, *tcprhskip();
	tcpseq		seq;
	int		cc;

	if (ptcb->tcb_ruhq >= 0)
		puqe = (struct uqe *)deq(ptcb->tcb_ruhq);
	else
		puqe = 0;
	seq = ptcb->tcb_rnext - ptcb->tcb_rbcount; /* start sequence	*/
	for (cc=0; ptcb->tcb_rbcount && cc < len;) {
		/* see if we're at an urgent data hole */
		if (puqe && SEQCMP(puqe->uq_seq, seq) <= 0) {
			puqe = tcprhskip(ptcb, puqe, seq);
			continue;
		}
		/* ...now normal data processing */

		*pch++ = ptcb->tcb_rcvbuf[ptcb->tcb_rbstart];
		--ptcb->tcb_rbcount;
		if (++ptcb->tcb_rbstart >= ptcb->tcb_rbsize)
			ptcb->tcb_rbstart = 0;
		++cc;
	}
	if (puqe)
		if (enq(ptcb->tcb_ruhq,puqe,RUHK(ptcb,puqe->uq_seq)) < 0)
			uqfree(puqe);	/* shouldn't happen... */
	if (ptcb->tcb_rbcount == 0)
		ptcb->tcb_flags &= ~TCBF_PUSH;
	/*
	 * open the receive window, if it's closed and we've made
	 * enough space to fit a segment.
	 */
	if (SEQCMP(ptcb->tcb_cwin, ptcb->tcb_rnext) <= 0 &&
			tcprwindow(ptcb)) {
		ptcb->tcb_flags |= TCBF_NEEDOUT;
		tcpkick(ptcb);
	}
	return cc;
}
