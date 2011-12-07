/* rtadd.c - rtadd */

#include <conf.h>
#include <kernel.h>
#include <network.h>

struct	route *rtnew();

/*------------------------------------------------------------------------
 *  rtadd  -  add a route to the routing table
 *------------------------------------------------------------------------
 */
int rtadd(net, mask, gw, metric, intf, ttl)
IPaddr	net, mask, gw;
int	metric, intf, ttl;
{
	struct	route	*prt, *srt, *prev;
	Bool		isdup;
	int		hv, i, j;

    if (Route.ri_valid != TRUE)
		rtinit();
	wait(Route.ri_mutex);
    prt = rtnew(net, mask, gw, metric, intf, ttl);
    if (prt == (struct route *)SYSERR) {
        xinu_signal(Route.ri_mutex);
		return SYSERR;
    }

	/* compute the queue sort key for this route */
	for (prt->rt_key = 0, i=0; i<IP_ALEN; ++i)
		for (j=0; j<8; ++j)
			prt->rt_key += (mask[i] >> j) & 1;

	/* special case for default routes */
	if (blkequ(net, RT_DEFAULT, IP_ALEN)) {
		if (Route.ri_default)
			RTFREE(Route.ri_default);
		Route.ri_default = prt;
		xinu_signal(Route.ri_mutex);
		return OK;
	}
	prev = NULL;
	hv = rthash(net);
	isdup = FALSE;
	for (srt=rttable[hv]; srt; srt = srt->rt_next) {
		if (prt->rt_key > srt->rt_key)
			break;
		if (blkequ(srt->rt_net, prt->rt_net, IP_ALEN) &&
	    	    blkequ(srt->rt_mask, prt->rt_mask, IP_ALEN)) {
			isdup = TRUE;
			break;
		}
		prev = srt;
	}
	if (isdup) {
		struct	route	*tmprt;

		if (blkequ(srt->rt_gw, prt->rt_gw, IP_ALEN)) {
			/* just update the existing route */
			if (dorip) {
				srt->rt_ttl = ttl;
				if (srt->rt_metric != metric) {
					if (metric == RTM_INF)
						srt->rt_ttl = RIPZTIME;
					send(rippid, 0);
				}
			}
			srt->rt_metric = metric;
			RTFREE(prt);
			xinu_signal(Route.ri_mutex);
			return OK;
		}
		/* else, someone else has a route there... */
		if (srt->rt_metric <= prt->rt_metric) {
			/* no better off to change; drop the new one */

			RTFREE(prt);
			xinu_signal(Route.ri_mutex);
			return OK;
		} else if (dorip)
			send(rippid, 0);
		tmprt = srt;
		srt = srt->rt_next;
		RTFREE(tmprt);
	} else if (dorip)
		send(rippid, 0);
	prt->rt_next = srt;
	if (prev)
		prev->rt_next = prt;
	else
		rttable[hv] = prt;
	xinu_signal(Route.ri_mutex);
	return OK;
}
