/* tcpreset.c - tcpreset */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  tcpreset -  generate a reset in response to a bad packet
 *------------------------------------------------------------------------
 */
int tcpreset(pepin)
struct	ep	*pepin;
{
	struct	ep	*pepout;
	struct	ip	*pipin = (struct ip *)pepin->ep_data, *pipout;
	struct	tcp	*ptcpin = (struct tcp *)pipin->ip_data, *ptcpout;
	int		datalen;

	if (ptcpin->tcp_code & TCPF_RST)
		return OK;		/* no RESETs on RESETs */
	pepout = (struct ep *)getbuf(Net.netpool);
    if (pepout == NULL)
		return SYSERR;
	pipout = (struct ip *)pepout->ep_data;
	blkcopy(pipout->ip_src, pipin->ip_dst, IP_ALEN);
	blkcopy(pipout->ip_dst, pipin->ip_src, IP_ALEN);
	pipout->ip_proto = IPT_TCP;
	pipout->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
	pipout->ip_len = IPMHLEN + TCPMHLEN;

	ptcpout = (struct tcp *)pipout->ip_data;
	ptcpout->tcp_sport = ptcpin->tcp_dport;
	ptcpout->tcp_dport = ptcpin->tcp_sport;
	if (ptcpin->tcp_code & TCPF_ACK) {
		ptcpout->tcp_seq = ptcpin->tcp_ack;
		ptcpout->tcp_code = TCPF_RST;
	} else {
		ptcpout->tcp_seq = 0;
		ptcpout->tcp_code = TCPF_RST|TCPF_ACK;
	}
	datalen = pipin->ip_len - IP_HLEN(pipin) - TCP_HLEN(ptcpin);
	if (ptcpin->tcp_code & TCPF_FIN)
		datalen++;
	ptcpout->tcp_ack = ptcpin->tcp_seq + datalen;
	ptcpout->tcp_offset = TCPHOFFSET;
	ptcpout->tcp_window = ptcpout->tcp_urgptr = 0;
	tcph2net(ptcpout);
	ptcpout->tcp_cksum = 0;
	ptcpout->tcp_cksum = tcpcksum(pipout);
	TcpOutSegs++;
	return ipsend(pipout->ip_dst, pepout, TCPMHLEN);
}
