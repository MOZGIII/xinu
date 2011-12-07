/* ipproc.c - ipproc */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>
#include <dos.h>

struct  ep      *ipgetp();
struct	route	*rtget();

/*------------------------------------------------------------------------
 *  ipproc  -  handle an IP datagram coming in from the network
 *------------------------------------------------------------------------
 */
PROCESS ipproc()
{
	struct	ep	*pep;
	struct	ip	*pip;
	struct	route	*prt;
	Bool		nonlocal;
	int		ifnum, rdtype;

	ippid = xinu_getpid();	/* so others can find us	*/

	xinu_signal(Net.sema);	/* signal initialization done	*/

	while (TRUE) {
		pep = ipgetp(&ifnum);
        pip = (struct ip *)pep->ep_data;
		if ((pip->ip_verlen>>4) != IP_VERSION) {
			IpInHdrErrors++;
			freebuf(pep);
			continue;
		}
		if (IP_CLASSD(pip->ip_dst) || IP_CLASSE(pip->ip_dst)) {
			IpInAddrErrors++;
			freebuf(pep);
			continue;
		}
		if (ifnum != NI_LOCAL) {
			if (cksum(pip, IP_HLEN(pip)>>1)) {
				IpInHdrErrors++;
				freebuf(pep);
				continue;
			}
			ipnet2h(pip);
		}
        prt = rtget(pip->ip_dst, (ifnum == NI_LOCAL));
		if (prt == NULL) {
			if (gateway) {
				iph2net(pip);
				icmp(ICT_DESTUR, ICC_NETUR,
						pip->ip_src, pep);
			} else {
				IpOutNoRoutes++;
				freebuf(pep);
			}
			continue;
		}
		nonlocal = ifnum != NI_LOCAL && prt->rt_ifnum != NI_LOCAL;
		if (!gateway && nonlocal) {
			IpInAddrErrors++;
			freebuf(pep);
			rtfree(prt);
			continue;
		}
		if (nonlocal)
			IpForwDatagrams++;
		/* fill in src IP, if we're the sender */

		if (ifnum == NI_LOCAL)
			if (blkequ(pip->ip_src, ip_anyaddr, IP_ALEN))
				if (prt->rt_ifnum == NI_LOCAL)
					blkcopy(pip->ip_src, pip->ip_dst,
						IP_ALEN);
				else
					blkcopy(pip->ip_src,
						nif[prt->rt_ifnum].ni_ip,
						IP_ALEN);
		if (--(pip->ip_ttl) == 0 && prt->rt_ifnum != NI_LOCAL) {
			IpInHdrErrors++;
			iph2net(pip);
            icmp(ICT_TIMEX, ICC_TIMEX, pip->ip_src, pep);
			rtfree(prt);
			continue;
		}
		ipdbc(ifnum, pep, prt);	/* handle directed broadcasts	*/
		ipredirect(pep, ifnum, prt); /* do redirect, if needed	*/
        if (prt->rt_metric != 0)
			ipputp(prt->rt_ifnum, prt->rt_gw, pep);
		else
			ipputp(prt->rt_ifnum, pip->ip_dst, pep);
		rtfree(prt);
	}
}

int	ippid, gateway, bsdbrc;
