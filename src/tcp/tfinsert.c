/* tfinsert.c - tfinsert */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <q.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  tfinsert - add a new TCP segment fragment to a TCB sequence queue
 *------------------------------------------------------------------------
 */
int	tfinsert(ptcb, seq, datalen, gotfin)
struct	tcb	*ptcb;
tcpseq		seq;
int		datalen;
{
	struct	tcpfrag	*tf;

	if (datalen == 0)
		return OK;
	tf = (struct tcpfrag *)getmem(sizeof(struct tcpfrag));
	tf->tf_seq = seq;
	tf->tf_len = datalen;
	if (ptcb->tcb_rsegq < 0)
		ptcb->tcb_rsegq = newq(NTCPFRAG, QF_WAIT);
	if (enq(ptcb->tcb_rsegq, tf, -tf->tf_seq) < 0)
		freemem((char*)tf, sizeof(struct tcpfrag));
	return OK;
}
