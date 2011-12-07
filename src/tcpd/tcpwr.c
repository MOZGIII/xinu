/* tcpwr.c - tcpwr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpwr  -  write urgent and normal data to TCP buffers
 *------------------------------------------------------------------------
 */
int tcpwr(pdev, pch, len, isurg)
struct	devsw	*pdev;
char		*pch;
int         len;
int         isurg;
{
	struct	tcb	*ptcb = (struct tcb *)pdev->dvioblk;
	int		state = ptcb->tcb_state;
    int     tocopy;
    int     sboff;

    if (state != TCPS_ESTABLISHED && state != TCPS_CLOSEWAIT)
		return SYSERR;
	tocopy = tcpgetspace(ptcb, len);	/* acquires tcb_mutex	*/
    if (tocopy <= 0)
		return tocopy;
	sboff = (ptcb->tcb_sbstart+ptcb->tcb_sbcount) % ptcb->tcb_sbsize;
	if (isurg)
        len = tcpwurg(ptcb, (long) sboff, len);
	while (tocopy--) {
		ptcb->tcb_sndbuf[sboff] = *pch++;
		++ptcb->tcb_sbcount;
		if (++sboff >= ptcb->tcb_sbsize)
			sboff = 0;
	}
	ptcb->tcb_flags |= TCBF_NEEDOUT;
	tcpwakeup(WRITERS, ptcb);
	xinu_signal(ptcb->tcb_mutex);
	if (isurg || ptcb->tcb_snext == ptcb->tcb_suna)
		tcpkick(ptcb);
	return len;
}
