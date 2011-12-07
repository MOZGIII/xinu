/* rttimer.c - rttimer */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <mem.h>

extern	Bool	dorip;		/* TRUE if we're running RIP output	*/
extern  int     rippid;     /* RIP output pid, if running       */

/*------------------------------------------------------------------------
 * rttimer - update ttls and delete expired routes
 *------------------------------------------------------------------------
 */
int rttimer(delta)
{
	struct	route	*prt, *prev;
	Bool		ripnotify;
	int		i;

	if (!Route.ri_valid)
		return;
	wait(Route.ri_mutex);
	ripnotify = FALSE;
	for (i=0; i<RT_TSIZE; ++i) {
        if (rttable[i] == NULL)
			continue;
		for (prev = NULL, prt = rttable[i]; prt != NULL;) {
			if (prt->rt_ttl != RT_INF)
				prt->rt_ttl -= delta;
			if (prt->rt_ttl <= 0) {
				if (dorip && prt->rt_metric < RTM_INF) {
					prt->rt_metric = RTM_INF;
					prt->rt_ttl = RIPZTIME;
					ripnotify = TRUE;
					continue;
				}
				if (prev) {
					prev->rt_next = prt->rt_next;
					RTFREE(prt);
					prt = prev->rt_next;
				} else {
					rttable[i] = prt->rt_next;
					RTFREE(prt);
					prt = rttable[i];
				}
				continue;
			}
			prev = prt;
			prt = prt->rt_next;
		}
	}
	prt = Route.ri_default;
	if (prt && (prt->rt_ttl<RT_INF) && (prt->rt_ttl -= delta) <= 0)
		if (dorip && prt->rt_metric < RTM_INF) {
			prt->rt_metric = RTM_INF;
			prt->rt_ttl = RIPZTIME;
		} else {
			RTFREE(Route.ri_default);
            Route.ri_default = NULL ;
		}
	xinu_signal(Route.ri_mutex);
    if (dorip && ripnotify) 
        send(rippid, 0);    /* send anything but TIMEOUT    */
    return OK;
}
