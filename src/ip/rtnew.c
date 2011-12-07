/* rtnew.c - rtnew */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  rtnew  -  create a route structure
 *------------------------------------------------------------------------
 */
struct route *rtnew(net, mask, gw, metric, ifnum, ttl)
IPaddr	net, mask, gw;
int	metric, ifnum, ttl;
{
	struct	route *prt;

	prt = (struct route *)getbuf(Route.ri_bpool);
	if (prt == (struct route *)SYSERR)
		return (struct route *)SYSERR;

	blkcopy(prt->rt_net, net, IP_ALEN);
	blkcopy(prt->rt_mask, mask, IP_ALEN);
	blkcopy(prt->rt_gw, gw, IP_ALEN);
	prt->rt_metric = metric;
	prt->rt_ifnum = ifnum;
	prt->rt_ttl = ttl;
	prt->rt_refcnt = 1;	/* our caller */
	prt->rt_usecnt = 0;
	prt->rt_next = NULL;
	return prt;
}
