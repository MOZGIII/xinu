/* tcpstat.c - tcpstat */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpstat - return status information for a TCP pseudo device
 *------------------------------------------------------------------------
 */
int tcpstat(ptcb, tcps)
struct	tcb	*ptcb;
struct tcpstat	*tcps;
{
	tcps->ts_type = ptcb->tcb_type;
	switch (ptcb->tcb_type) {
	case TCPT_SERVER:
		/* should increase to entire TCP MIB */
		tcps->ts_connects = TcpActiveOpens;
		tcps->ts_aborts = TcpEstabResets;
		tcps->ts_retrans = TcpRetransSegs;
		break;
	case TCPT_CONNECTION:
		blkcopy(tcps->ts_laddr, ptcb->tcb_lip, IP_ALEN);
		tcps->ts_lport = ptcb->tcb_lport;
		blkcopy(tcps->ts_faddr, ptcb->tcb_rip, IP_ALEN);
		tcps->ts_fport = ptcb->tcb_rport;
		tcps->ts_rwin = ptcb->tcb_rbsize - ptcb->tcb_rbcount; 
		tcps->ts_swin = ptcb->tcb_swindow;
		tcps->ts_state = ptcb->tcb_state;
		tcps->ts_unacked = ptcb->tcb_suna;
		tcps->ts_prec = 0;
		break;
	case TCPT_MASTER:
		break;
	}
	return OK;
}
