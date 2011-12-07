/* local_out.c - local_out */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

struct ep	*ipreass();

/*------------------------------------------------------------------------
 *  local_out  -  handle an IP datagram headed for a local process
 *------------------------------------------------------------------------
 */
int local_out(pep)
struct	ep	*pep;
{
	struct	netif	*pni = &nif[NI_LOCAL];
	struct	ip	*pip = (struct ip *)pep->ep_data;
	int		rv;

	ipnet2h(pip);
	pep = ipreass(pep);
    if (pep == NULL)
		return OK;
	pip = (struct ip *)pep->ep_data;
	ipdstopts(pni, pep);		/* do IP option processing	*/
    switch (pip->ip_proto) {
	case IPT_UDP:
		rv = udp_in(pni, pep);
		break;
	case IPT_ICMP:
		rv = icmp_in(pni, pep);
		break;
	case IPT_TCP:
		rv = tcp_in(pni, pep);
		break;
	default:
        IpInUnknownProtos++;
		icmp(ICT_DESTUR, ICC_PROTOUR, pip->ip_src, pep);
		return OK;
	}
	IpInDelivers++;
	return rv;
}
