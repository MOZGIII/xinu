/* x_routes.c - x_routes */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_routes  -  (command routes) format and print routing entries
 *------------------------------------------------------------------------
 */
COMMAND x_routes(nargs, args)
int     nargs;
char	*args[];
{
	char netname[64], gwname[64];
	char str[80];
	struct route *srt;
	int	i, usedots;

	if (nargs > 2) {
        write (STDOUT, "usage: routes [-n]\n", 19);
		return OK;
	}
    usedots = nargs != 2; 
	
	xinu_sprintf(str, "%-16s %-8s %-16s metric intf ttl refcnt usecnt\n",
            "destination", "mask", "nexthop");
    xinu_write(STDOUT, str, xinu_strlen(str));
	for(i=0; i<RT_TSIZE; ++i) {
		for (srt=rttable[i]; srt != NULL; srt=srt->rt_next) {
			if (usedots) {
				ip2dot(netname, srt->rt_net);
				ip2dot(gwname, srt->rt_gw);
			} else {
				ip2name(srt->rt_net, netname);
				ip2name(srt->rt_gw, gwname);
			}
			netname[16] = gwname[16] = '\0';
            xinu_sprintf(str, "%-16s %02x%02x%02x%02x %-16s %6d   %1d  ",
                netname, srt->rt_mask[0], srt->rt_mask[1],
                srt->rt_mask[2], srt->rt_mask[3], gwname,
				srt->rt_metric, srt->rt_ifnum);
            xinu_write(STDOUT, str, xinu_strlen(str));
			if (srt->rt_ttl >= RT_INF)
				xinu_sprintf(str, " -  %6d %6d\n",
					srt->rt_refcnt, srt->rt_usecnt);
			else
				xinu_sprintf(str, "%3d %6d %6d\n", srt->rt_ttl,
					srt->rt_refcnt, srt->rt_usecnt);
            xinu_write(STDOUT, str, xinu_strlen(str));
		}
	}
	if (Route.ri_default) {
		srt = Route.ri_default;
		if (usedots) {
			ip2dot(netname, srt->rt_net);
			ip2dot(gwname, srt->rt_gw);
		} else {
			ip2name(srt->rt_net, netname);
			ip2name(srt->rt_gw, gwname);
		}
		netname[16] = gwname[16] = '\0';
            xinu_sprintf(str, "%-16s %02x%02x%02x%02x %-16s %6d   %1d  ",
                netname, srt->rt_mask[0], srt->rt_mask[1],
                srt->rt_mask[2], srt->rt_mask[3], gwname,
				srt->rt_metric, srt->rt_ifnum);
        xinu_write(STDOUT, str, xinu_strlen(str));
		if (srt->rt_ttl >= RT_INF)
			xinu_sprintf(str, " -  %6d %6d\n",
				srt->rt_refcnt, srt->rt_usecnt);
		else
			xinu_sprintf(str, "%3d %6d %6d\n", srt->rt_ttl,
				srt->rt_refcnt, srt->rt_usecnt);
        xinu_write(STDOUT, str, xinu_strlen(str));
	}
}
