/* udpsend.c - udpsend */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  udpsend  -  send one UDP datagram to a given IP address
 *------------------------------------------------------------------------
 */
int
udpsend(fip, fport, lport, pep, datalen, docksum)
IPaddr		fip;
unsigned short	fport, lport;
struct	ep	*pep;
int		datalen;
int     docksum;  /* was Bool */
{
	struct	ip	*pip = (struct ip *) pep->ep_data;
	struct	udp	*pudp = (struct udp *) pip->ip_data;
	struct	route	*prt, *rtget();

	pip->ip_proto = IPT_UDP;
	pudp->u_src = lport;
	pudp->u_dst = fport;
	pudp->u_len = U_HLEN+datalen;
	pudp->u_cksum = 0;
	udph2net(pudp);
    if (docksum) {
		prt = rtget(fip, RTF_LOCAL);
		if (prt == NULL) {
			IpOutNoRoutes++;
			freebuf(pep);
			return SYSERR;
		}
		blkcopy(pip->ip_dst, fip, IP_ALEN);
		if (prt->rt_ifnum == NI_LOCAL)
			blkcopy(pip->ip_src, pip->ip_dst, IP_ALEN);
		else
			blkcopy(pip->ip_src, nif[prt->rt_ifnum].ni_ip,
				IP_ALEN);
		rtfree(prt);
		pudp->u_cksum = udpcksum(pip);
		if (pudp->u_cksum == 0)
			pudp->u_cksum = ~0;
	}
	UdpOutDatagrams++;
	return ipsend(fip, pep, U_HLEN+datalen);
}
