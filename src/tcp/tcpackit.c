/* tcpackit.c - tcpackit */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  tcpackit -  generate an ACK for a received TCP packet
 *------------------------------------------------------------------------
 */
int tcpackit(ptcb, pepin)
struct	tcb	*ptcb;
struct	ep	*pepin;
{
	struct ep	*pepout;
	struct ip	*pipin = (struct ip *)pepin->ep_data, *pipout;
	struct tcp	*ptcpin = (struct tcp *)pipin->ip_data, *ptcpout;

	if (ptcpin->tcp_code & TCPF_RST)
		return OK;
	if (pipin->ip_len <= IP_HLEN(pipin) + TCP_HLEN(ptcpin) &&
			!(ptcpin->tcp_code & (TCPF_SYN|TCPF_FIN)))
		return OK;	/* duplicate ACK */
	pepout = (struct ep *)getbuf(Net.netpool);
    if (pepout == NULL)
		return SYSERR;
	pipout = (struct ip *)pepout->ep_data;
	blkcopy(pipout->ip_src, pipin->ip_dst, IP_ALEN);
	blkcopy(pipout->ip_dst, pipin->ip_src, IP_ALEN);
	pipout->ip_proto = IPT_TCP;
	pipout->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
	pipout->ip_len = IP_HLEN(pipout) + TCPMHLEN;
	ptcpout = (struct tcp *)pipout->ip_data;
	ptcpout->tcp_sport = ptcpin->tcp_dport;
	ptcpout->tcp_dport = ptcpin->tcp_sport;
	ptcpout->tcp_seq = ptcb->tcb_snext;
	ptcpout->tcp_ack = ptcb->tcb_rnext;
	ptcpout->tcp_code = TCPF_ACK;
	ptcpout->tcp_offset = TCPHOFFSET;
	ptcpout->tcp_window = tcprwindow(ptcb);
    ptcpout->tcp_urgptr = 0;
	ptcpout->tcp_cksum = 0;
	tcph2net(ptcpout);
	ptcpout->tcp_cksum = tcpcksum(pipout);
	TcpOutSegs++;
	return ipsend(pipout->ip_dst, pepout, TCPMHLEN);
}
