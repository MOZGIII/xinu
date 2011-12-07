/* tcpread.c - tcpread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpread  -  read one buffer from a TCP pseudo-device
 *------------------------------------------------------------------------
 */
tcpread(pdev, pch, len)
struct	devsw	*pdev;
char		*pch;
int		len;
{
	struct	tcb	*ptcb = (struct tcb *)pdev->dvioblk;
	int		state = ptcb->tcb_state;
	int		cc;

	if (state != TCPS_ESTABLISHED && state != TCPS_CLOSEWAIT)
		return SYSERR;
retry:
	wait(ptcb->tcb_rsema);
	wait(ptcb->tcb_mutex);

	if (ptcb->tcb_state == TCPS_FREE)
		return SYSERR;			/* gone		*/
	if (ptcb->tcb_error) {
		tcpwakeup(READERS, ptcb);	/* propagate it */
		xinu_signal(ptcb->tcb_mutex);
		return ptcb->tcb_error;
	}
	if (ptcb->tcb_rudq < 0) {
		if (proctab[currpid].ptcpumode) {
			proctab[currpid].ptcpumode = FALSE;
			cc = TCPE_NORMALMODE;
		} else if (len > ptcb->tcb_rbcount &&
		    ptcb->tcb_flags & TCBF_BUFFER &&
		    (ptcb->tcb_flags & (TCBF_PUSH|TCBF_RDONE)) == 0) {
			xinu_signal(ptcb->tcb_mutex);
			goto retry;
		} else
			cc = tcpgetdata(ptcb, pch, len);
	} else {
		if (proctab[currpid].ptcpumode)
			cc = tcprurg(ptcb, pch, len);
		else {
			proctab[currpid].ptcpumode = TRUE;
			cc = TCPE_URGENTMODE;
		}
	}
	if (cc == 0 && (ptcb->tcb_flags & TCBF_RDONE) == 0) {
		/*
		 * Have to block after all. Holes can cause
		 * rbcount != 0, but no real data available.
		 */
		xinu_signal(ptcb->tcb_mutex);
		goto retry;
	}
	tcpwakeup(READERS, ptcb);
	xinu_signal(ptcb->tcb_mutex);
	return cc;
}
