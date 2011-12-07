/* tcpsend.c - tcpsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  tcpsend -  compute and send a TCP segment for the given TCB
 *------------------------------------------------------------------------
 */
int tcpsend(tcbnum, rexmt)
int	tcbnum;
int rexmt;
{
	struct	tcb	*ptcb = &tcbtab[tcbnum];
	struct	ep	*pep;
	struct	ip	*pip;
	struct	tcp	*ptcp;
	char		*pch;
	int		i, datalen, tocopy, off, newdata;

	pep = (struct ep *)getbuf(Net.netpool);
    if (pep == NULL)
		return SYSERR;
	pip = (struct ip *)pep->ep_data;
	blkcopy(pip->ip_src, ptcb->tcb_lip, IP_ALEN);
	blkcopy(pip->ip_dst, ptcb->tcb_rip, IP_ALEN);
	pip->ip_proto = IPT_TCP;
	datalen = tcpsndlen(ptcb, rexmt, &off);	/* get length & offset	*/
	ptcp = (struct tcp *)pip->ip_data;
	ptcp->tcp_sport = ptcb->tcb_lport;
	ptcp->tcp_dport = ptcb->tcb_rport;
	if (!rexmt) {
		if (ptcb->tcb_code & TCPF_URG)
			ptcp->tcp_seq = ptcb->tcb_suna + off;
		else
			ptcp->tcp_seq = ptcb->tcb_snext;
	} else
		ptcp->tcp_seq = ptcb->tcb_suna;
	ptcp->tcp_ack = ptcb->tcb_rnext;

	if ((ptcb->tcb_flags & TCBF_SNDFIN) &&
	    SEQCMP(ptcp->tcp_seq+datalen, ptcb->tcb_slast) == 0)
		ptcb->tcb_code |= TCPF_FIN;
	ptcp->tcp_code = ptcb->tcb_code;
	ptcp->tcp_offset = TCPHOFFSET;
	if ((ptcb->tcb_flags & TCBF_FIRSTSEND) == 0)
		ptcp->tcp_code |= TCPF_ACK;
	if (ptcp->tcp_code & TCPF_SYN)
		tcprmss(ptcb, pip);
	pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
	pip->ip_len = IP_HLEN(pip) + TCP_HLEN(ptcp) + datalen;
	if (datalen > 0)
		ptcp->tcp_code |= TCPF_PSH;
	ptcp->tcp_window = tcprwindow(ptcb);
	if (ptcb->tcb_code & TCPF_URG)
#ifdef	BSDURG
		ptcp->tcp_urgptr = datalen;	/* 1 past end		*/
#else	/* BSDURG */
		ptcp->tcp_urgptr = datalen-1;
#endif	/* BSDURG */
	else
		ptcp->tcp_urgptr = 0;
	pch = &pip->ip_data[TCP_HLEN(ptcp)];
	i = (ptcb->tcb_sbstart+off) % ptcb->tcb_sbsize;
	for (tocopy=datalen; tocopy > 0; --tocopy) {
		*pch++ = ptcb->tcb_sndbuf[i];
		if (++i >= ptcb->tcb_sbsize)
			i = 0;
	}
	ptcb->tcb_flags &= ~TCBF_NEEDOUT;	/* we're doing it	*/
	if (rexmt) {
		newdata = ptcb->tcb_suna + datalen - ptcb->tcb_snext;
		if (newdata < 0)
			newdata = 0;
		TcpRetransSegs++;
	} else {
		newdata = datalen;
		if (ptcb->tcb_code & TCPF_SYN)
			newdata++; /* SYN is part of the sequence	*/
		if (ptcb->tcb_code & TCPF_FIN)
			newdata++; /* FIN is part of the sequence	*/
	}
	ptcb->tcb_snext += newdata;
	if (newdata >= 0)
		TcpOutSegs++;
	if (ptcb->tcb_state == TCPS_TIMEWAIT)	/* final ACK		*/
		tcpwait(ptcb);
	datalen += TCP_HLEN(ptcp);
	tcph2net(ptcp);
	ptcp->tcp_cksum = 0;
	ptcp->tcp_cksum = tcpcksum(pip);
    return ipsend(pip->ip_dst, pep, datalen);
}
