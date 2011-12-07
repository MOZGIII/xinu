/* ipsend.c - ipsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static ipackid = 1;

/*------------------------------------------------------------------------
 *  ipsend  -  fill in IP header and send datagram to specified address
 *------------------------------------------------------------------------
 */
int ipsend(faddr, pep, datalen)
IPaddr		faddr;
struct	ep	*pep;
int		datalen;
{
	struct	ip *pip = (struct ip *) pep->ep_data;
	pep->ep_type = EPT_IP;
	pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
	pip->ip_tos = 0;
	pip->ip_len = datalen+IP_HLEN(pip);
    pip->ip_id = ipackid++;
	pip->ip_fragoff = 0;
	pip->ip_ttl = IP_TTL;
	blkcopy(pip->ip_dst, faddr, IP_ALEN);

	/*
	 * special case for ICMP, so source matches destination
	 * with multiple interfaces.
	 */
	if (pip->ip_proto != IPT_ICMP)
		blkcopy(pip->ip_src, ip_anyaddr, IP_ALEN);
	if (enq(nif[NI_LOCAL].ni_ipinq, pep, 0) < 0) {
        freebuf(pep);
		IpOutDiscards++;
	}
    send(ippid, NI_LOCAL);
	IpOutRequests++;
	return OK;
}
/* special IP addresses */

IPaddr	ip_anyaddr = { 0, 0, 0, 0 };
IPaddr	ip_loopback = { 127, 0, 0, 1 };
