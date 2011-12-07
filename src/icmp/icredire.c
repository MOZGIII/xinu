/* icredirect.c - icredirect */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

struct	route	*rtget();

/*------------------------------------------------------------------------
 *  icredirect -  handle an incoming ICMP redirect
 *------------------------------------------------------------------------
 */
int icredirect(pep)
struct	ep	*pep;
{
	struct	route	*prt;
	struct	ip	*pip, *pip2;
	struct	icmp	*pic;
	IPaddr		mask;

	pip = (struct ip *)pep->ep_data;
	pic = (struct icmp *)pip->ip_data;
	pip2 = (struct ip *)pic->ic_data;

	if (pic->ic_code == ICC_HOSTRD)
		blkcopy(mask, ip_maskall, IP_ALEN);
	else
		netmask(mask, pip2->ip_dst);
	prt = rtget(pip2->ip_dst, RTF_LOCAL);
	if (prt == 0) {
		freebuf(pep);
		return OK;
	}
	if (blkequ(pip->ip_src, prt->rt_gw, IP_ALEN)) {
		rtdel(pip2->ip_dst, mask);
		rtadd(pip2->ip_dst, mask, pic->ic_gw, prt->rt_metric,
			prt->rt_ifnum, IC_RDTTL);
	}
	rtfree(prt);
	freebuf(pep);
	return OK;
}
