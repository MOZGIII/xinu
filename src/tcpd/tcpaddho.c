/* tcpaddhole.c - tcpaddhole */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  tcpaddhole  -  add an URGENT data hole to the receive hole queue
 *------------------------------------------------------------------------
 */
int tcpaddhole(ptcb, puqe)
struct	tcb	*ptcb;
struct	uqe	*puqe;
{
	freebuf(puqe->uq_data, puqe->uq_len);
	puqe->uq_data = 0;
	if (ptcb->tcb_ruhq < 0)
		ptcb->tcb_ruhq = newq(TCPUQLEN, QF_WAIT);
	if (enq(ptcb->tcb_ruhq, puqe, RUHK(ptcb, puqe->uq_seq)) < 0)
		uqfree(puqe);
	return OK;
}
