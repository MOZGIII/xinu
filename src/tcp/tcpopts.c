/* tcpopts.c - tcpopts */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpopts - handle TCP options for an inbound segment
 *------------------------------------------------------------------------
 */
int tcpopts(ptcb, pep)
struct	tcb	*ptcb;
struct	ep	*pep;
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;
	char		*popt, *popend;
	int		len;

	if (TCP_HLEN(ptcp) == TCPMHLEN)
		return OK;
	popt = ptcp->tcp_data;
	popend = &pip->ip_data[TCP_HLEN(ptcp)];
	do {
		switch (*popt) {
		case TPO_NOOP:	popt++;
				/* fall through */
		case TPO_EOOL:	break;
		case TPO_MSS:
			popt += tcpsmss(ptcb, ptcp, popt);
			break;
		default:
			break;
		}
	} while (*popt != TPO_EOOL && popt<popend);

	/* delete the options */
	len = pip->ip_len-IP_HLEN(pip)-TCP_HLEN(ptcp);
	if (len)
		blkcopy(ptcp->tcp_data,&pip->ip_data[TCP_HLEN(ptcp)],len);
	pip->ip_len = IP_HLEN(pip) + TCPMHLEN + len;
	ptcp->tcp_offset = TCPHOFFSET;
	return OK;
}
