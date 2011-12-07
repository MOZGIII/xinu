/* tcpsndurg.c - tcpsndurg */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpsndurg.c - send pending urgent data
 *------------------------------------------------------------------------
 */
int tcpsndurg(tcbnum)
int	tcbnum;
{
	struct tcb	*ptcb = &tcbtab[tcbnum];
	int	tv;

	ptcb->tcb_code |= TCPF_URG;
	while (ptcb->tcb_sudq > 0)
		tcpsend(tcbnum, TSF_NEWDATA);
	ptcb->tcb_code &= ~TCPF_URG;
	return OK;
}
