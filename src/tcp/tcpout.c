/* tcpout.c - tcpout */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  tcpout - handle events affecting TCP output processing
 *------------------------------------------------------------------------
 */
PROCESS tcpout()
{
	struct	tcb	*ptcb;
    long     i;

	tcps_oport = pcreate(TCPQLEN);
	xinu_signal(Net.sema);		/* synchronize on startup	*/

	while (TRUE) {
		i = (long)preceive(tcps_oport);
        ptcb = &tcbtab[TCB(i)];
        if (ptcb->tcb_state <= TCPS_CLOSED)
			continue;		/* a rogue; ignore it	*/
		wait(ptcb->tcb_mutex);
		if (ptcb->tcb_state <= TCPS_CLOSED)
			continue;		/* TCB deallocated	*/
		if (EVENT(i) == DELETE)		/* same for all states	*/
			tcbdealloc(ptcb);
        else 
            tcposwitch[ptcb->tcb_ostate]((int)(TCB(i)), ((int)EVENT(i)));
		if (ptcb->tcb_state != TCPS_FREE)
			xinu_signal(ptcb->tcb_mutex);
	}
}
