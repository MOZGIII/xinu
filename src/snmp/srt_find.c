/* srt_findn.c - srt_findnext */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 * srt_findnext - find next route in the lexicographic ordering
 *------------------------------------------------------------------------
 */
struct route *srt_findnext(rtp, rtl)
struct route 	*rtp;
int 		*rtl;
{
	struct route 	*nextrtp, *trtp;
	int 		i, nextrtl;
	
	for (i = 0, nextrtl = -1; i < RT_TSIZE; i++)
		for (trtp = rttable[i]; trtp; trtp = trtp->rt_next) {
			if (*rtl >= 0 &&
			    blkcmp(trtp->rt_net,rtp->rt_net,IP_ALEN)<=0)
				continue;
			if (nextrtl < 0 || blkcmp(trtp->rt_net,
					nextrtp->rt_net, IP_ALEN) < 0) {
				nextrtp = trtp;
				nextrtl = i;
			}
		}
	if (nextrtl == -1)	/* no next route found */
		return 0;
	*rtl = nextrtl;
	return nextrtp;
}
