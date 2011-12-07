/* rtget.c - rtget */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rtget  -  get the route for a given IP destination
 *------------------------------------------------------------------------
 */
struct route *rtget(dest, local)
IPaddr	dest;
Bool    local;      /* TRUE <=> locally generated traffic */
{
	struct	route	*prt;
    int     hv;

	if (!Route.ri_valid)
		rtinit();
	wait(Route.ri_mutex);
    hv = rthash(dest);
	for (prt=rttable[hv]; prt; prt=prt->rt_next) {
        if (prt->rt_ttl <= 0)
			continue;		/* route has expired */
        if (netmatch(dest, prt->rt_net, prt->rt_mask, local))
			if (prt->rt_metric < RTM_INF)
				break;
	}
    if (prt == NULL)
		prt = Route.ri_default;	/* may be NULL too... */
    if (prt != NULL && prt->rt_metric >= RTM_INF)
        prt = NULL;
	if (prt) {
        prt->rt_refcnt++;
        prt->rt_usecnt++;
	}
	xinu_signal(Route.ri_mutex);
	return prt;
}
