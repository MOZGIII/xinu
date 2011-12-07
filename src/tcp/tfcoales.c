/* tfcoalesce.c - tfcoalesce */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>

/*------------------------------------------------------------------------
 *  tfcoalesce -  join TCP fragments
 *------------------------------------------------------------------------
 */
int tfcoalesce(ptcb, datalen, ptcp)
struct	tcb	*ptcb;
int		datalen;
struct	tcp	*ptcp;
{
	struct tcpfrag	*tf;
	int	new;

	ptcb->tcb_rnext += datalen;
	ptcb->tcb_rbcount += datalen;
	if (ptcb->tcb_rnext == ptcb->tcb_finseq)
		goto alldone;
	if ((ptcb->tcb_rnext - ptcb->tcb_pushseq) >= 0) {
		ptcp->tcp_code |= TCPF_PSH;
		ptcb->tcb_pushseq = 0;
	}
	if (ptcb->tcb_rsegq < 0)	/* see if this closed a hole */
		return OK;
	tf = (struct tcpfrag *)deq(ptcb->tcb_rsegq);
	while ((tf->tf_seq - ptcb->tcb_rnext) <= 0) {
		new = tf->tf_len - (ptcb->tcb_rnext - tf->tf_seq);
		if (new > 0) {
			ptcb->tcb_rnext += new;
			ptcb->tcb_rbcount += new;
		}
		if (ptcb->tcb_rnext == ptcb->tcb_finseq)
			goto alldone;
		if ((ptcb->tcb_rnext - ptcb->tcb_pushseq) >= 0) {
			ptcp->tcp_code |= TCPF_PSH;
			ptcb->tcb_pushseq = 0;
		}
		freemem(tf, sizeof(struct tcpfrag));
		tf = (struct tcpfrag *)deq(ptcb->tcb_rsegq);
		if (tf == 0) {
			freeq(ptcb->tcb_rsegq);
			ptcb->tcb_rsegq = EMPTY;
			return OK;
		}
	}
	enq(ptcb->tcb_rsegq, tf, -tf->tf_seq); /* got one too many	*/
	return OK;
alldone:
	do
		freemem(tf, sizeof(struct tcpfrag));
	while (tf=(struct tcpfrag *)deq(ptcb->tcb_rsegq));
	freeq(ptcb->tcb_rsegq);
	ptcb->tcb_rsegq = EMPTY;
	ptcp->tcp_code |= TCPF_FIN;
	return OK;
}
