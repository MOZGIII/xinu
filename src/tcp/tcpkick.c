/* tcpkick.c - tcpkick */

#include <conf.h>
#include <kernel.h>
#include <network.h>

int psend(int, long);
int tmset(int, int, long, long);

/*------------------------------------------------------------------------
 *  tcpkick -  make sure we send a packet soon
 *------------------------------------------------------------------------
 */
int tcpkick(ptcb)
struct	tcb	*ptcb;
{
	int	tcbnum = ptcb - &tcbtab[0];	/* for MKEVENT() */
    long tv;

	tv = MKEVENT(SEND, tcbnum);
    if (ptcb->tcb_flags & TCBF_DELACK && !tmleft(tcps_oport, tv))
        tmset(tcps_oport, TCPQLEN, tv, (long)TCP_ACKDELAY);
	else if (pcount(tcps_oport) < TCPQLEN)
		psend(tcps_oport, tv);	/* send now */
	return OK;
}
