/* tcpcntl.c - tcpcntl */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ports.h>

/*------------------------------------------------------------------------
 *  tcpcntl - control function for TCP pseudo-devices
 *------------------------------------------------------------------------
 */
int tcpcntl(pdev, func, arg, arg2)
struct	devsw	*pdev;
int		func;
char		*arg, *arg2;
{
	struct	tcb	*ptcb = (struct tcb *)pdev->dvioblk;
	int		rv;

	if (ptcb == NULL || ptcb->tcb_state == TCPS_FREE)
		return SYSERR;

	wait(ptcb->tcb_mutex);
	if (ptcb->tcb_state == TCPS_FREE) /* verify no state change */
		return SYSERR;

	switch (func) {
	case TCPC_ACCEPT:	if (ptcb->tcb_type != TCPT_SERVER) {
					rv = SYSERR;
					break;
				}
                xinu_signal(ptcb->tcb_mutex);
                return (int) preceive(ptcb->tcb_listenq);
	case TCPC_LISTENQ:	rv = tcplq(ptcb, arg);
				break;
	case TCPC_STATUS:	rv = tcpstat(ptcb, arg);
				break;
	case TCPC_SOPT:
	case TCPC_COPT:		rv = tcpuopt(ptcb, func, arg);
				break;
	case TCPC_SENDURG:	/*
				 * tcpwr acquires and releases tcb_mutex
				 * itself.
				 */
				xinu_signal(ptcb->tcb_mutex);
                return tcpwr(pdev, arg, ((int) arg2), TWF_URGENT);
	default:
		rv = SYSERR;
	}
	xinu_signal(ptcb->tcb_mutex);
	return rv;
}
