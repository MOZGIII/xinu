/* rtdel.c - rtdel */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rtdel  -  delete the route with the given net, mask
 *------------------------------------------------------------------------
 */
int rtdel(net, mask)
IPaddr	net, mask;		/* destination network and mask		*/
{
	struct	route	*prt, *prev;
	int		hv, i;

	if (!Route.ri_valid)
		return SYSERR;
	wait(Route.ri_mutex);
	if (Route.ri_default &&
	    blkequ(net, Route.ri_default->rt_net, IP_ALEN)) {
		RTFREE(Route.ri_default);
        Route.ri_default = NULL;
		xinu_signal(Route.ri_mutex);
		return OK;
	}
	hv = rthash(net);

	prev = NULL;
	for (prt = rttable[hv]; prt; prt = prt->rt_next) {
		if (blkequ(net, prt->rt_net, IP_ALEN) &&
		    blkequ(mask, prt->rt_mask, IP_ALEN))
			break;
		prev = prt;
	}
	if (prt == NULL) {
		xinu_signal(Route.ri_mutex);
		return SYSERR;
	}
	if (prev)
		prev->rt_next = prt->rt_next;
	else
		rttable[hv] = prt->rt_next;
	RTFREE(prt);
	xinu_signal(Route.ri_mutex);
	return OK;
}
