/* tcpmopen.c - tcpmopen */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  tcpmopen  -  open a fresh TCP pseudo device and return descriptor
 *------------------------------------------------------------------------
 */
int tcpmopen(pdev, fport, lport)
struct	devsw	*pdev;
char		*fport;
int		lport;
{
    struct  tcb *ptcb;
    extern struct  tcb *tcballoc();
	int		error;

    ptcb = tcballoc();
    if (ptcb == (struct tcb *)SYSERR)
		return SYSERR;
	ptcb->tcb_error = 0;
	proctab[currpid].ptcpumode = FALSE;
	if (fport == ANYFPORT)
		return tcpserver(ptcb, lport);

	if (tcpbind(ptcb, fport, lport) != OK ||
	    tcpsync(ptcb) != OK) {
		ptcb->tcb_state = TCPS_FREE;
		sdelete(ptcb->tcb_mutex);
		return SYSERR;
	}
    if (error = tcpcon(ptcb))
		return error;
    return ptcb->tcb_dvnum;
}
