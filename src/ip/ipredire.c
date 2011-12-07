/* ipredirect.c - ipredirect */

#include <conf.h>
#include <kernel.h>
#include <network.h>

struct	route	*rtget();

/*------------------------------------------------------------------------
 *  ipredirect  -  send redirects, if needed
 *------------------------------------------------------------------------
 */
void ipredirect(pep, ifnum, prt)
struct	ep	*pep;		/* the current IP packet		*/
int		ifnum;		/* the input interface			*/
struct	route	*prt;		/* where we want to route it		*/
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	route	*tprt;
	int		rdtype, isonehop;
	IPaddr		nmask;	/* network part's mask			*/

	if (ifnum == NI_LOCAL || ifnum != prt->rt_ifnum)
		return;
	tprt = rtget(pip->ip_src, RTF_LOCAL);
	if (!tprt)
		return;
	isonehop = tprt->rt_metric == 0;
	rtfree(tprt);
	if (!isonehop)
		return;
	/* got one... */

	netmask(nmask, prt->rt_net);	/* get the default net mask	*/
	if (blkequ(prt->rt_mask, nmask, IP_ALEN))
		rdtype = ICC_NETRD;
	else
		rdtype = ICC_HOSTRD;
    icmp(ICT_REDIRECT, rdtype, pip->ip_src, pep, prt->rt_gw);
}
