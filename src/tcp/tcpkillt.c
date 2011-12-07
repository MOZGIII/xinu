/* tcpkilltimers.c - tcpkilltimers */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpkilltimers -  kill all outstanding timers for a TCB
 *------------------------------------------------------------------------
 */
int tcpkilltimers(ptcb)
struct	tcb	*ptcb;
{
	int	tcbnum = ptcb - &tcbtab[0];

	/* clear all possible pending timers */

    tmclear(tcps_oport, (long) MKEVENT(SEND, tcbnum));
    tmclear(tcps_oport, (long) MKEVENT(RETRANSMIT, tcbnum));
    tmclear(tcps_oport, (long) MKEVENT(PERSIST, tcbnum));
	return OK;
}
